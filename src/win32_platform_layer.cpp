// Following along with the Handmade youtube series
// Author: Tyler Wolf

// Need to get the types at the very beginning
#include <stdint.h>
// TODO: Implement sine ourselves?
#include <math.h>

#include <malloc.h>
#include <windows.h>
#include <dsound.h>

#include "win32_platform_layer.h"
#include "err.h"

// TODO: This is a global for now, probably should change later.
global_variable bool32 GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackBuffer;
global_variable int64_t GlobalPerfCountFrequency;
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;

// ------------------------- SOUND RELATED ITEMS -------------------------------------

// Initializes our DirectSound buffers so we can play audio
internal void
Win32InitDSound(HWND Window, int32_t SamplesPerSecond, int32_t BufferSize)
{
	// NOTE: Load the Library
	HMODULE DSoundLibrary = LoadLibrary("dsound.dll");
    
	if(DSoundLibrary)
	{
		// NOTE: Get a DirectSound Object - cooperative
		direct_sound_create *DirectSoundCreate = 
            (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");
        
		// TODO: Double check that this works on XP
		LPDIRECTSOUND DirectSound;
		if(DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
		{
            
			WAVEFORMATEX WaveFormat = {};
			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormat.nChannels = 2; // stereo
			WaveFormat.nSamplesPerSec = SamplesPerSecond;
			WaveFormat.wBitsPerSample = 16;
			// NOTE: Get size of single audio unit [LEFT RIGHT] in bytes
			WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
			WaveFormat.nAvgBytesPerSec = WaveFormat.nBlockAlign * WaveFormat.nSamplesPerSec;
			WaveFormat.cbSize = 0;
            
			if(SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
			{
				/************************************************
				 * 				CREATE PRIMARY BUFFER			*
				 ************************************************/
				// this ensures that the sound card is using the same wave format
				// as the audio we will be playing. This isn't really a buffer,
				// it's actually more of a handle to the sound card
				DSBUFFERDESC BufferDescription = {};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
                
				// TODO: Check if we want to use DSBCAPS_GLOBALFOCUS
				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
				{
					// Set the format for the sound card
					if(SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat)))
					{
						// NOTE: We've finally set the format!
						OutputDebugString("Primary buffer format was set.\n");
					}
					else
					{
						// TODO: Diagnostic - failed setting format on primary buffer
					}
				}
				else
				{
					// TODO: Diagnostic - Failed to Create Primary Buffer
				}
			}
			else
			{
				// TODO: Diagnostic - Failed to set cooperative level
			}
            
            
			/************************************************
			 * 				CREATE SECONDARY BUFFER			*
			 ************************************************/
			// This is the buffer we actually care about and write to
			DSBUFFERDESC BufferDescription = {};
			BufferDescription.dwSize = sizeof(BufferDescription);
			// TODO: maybe use BufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
			BufferDescription.dwBufferBytes = BufferSize;
			BufferDescription.lpwfxFormat = &WaveFormat;
			BufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
			if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0)))
			{
				// NOTE: Start it playing!
				OutputDebugString("Secondary buffer format was created.\n");
			}
			else
			{
				// TODO: Diagnostic - Failed to Create Secondary Buffer
			}
		}
		else
		{
			// TODO: Diagnostic - failed to direct sound create
		}
	}
	else
	{
		//TODO: Diagnostic - failed to load library
	}
}

internal void
Win32ClearBuffer(win32_sound_output *SoundOutput)
{
    VOID *Region1;
    DWORD Region1Size;
    VOID *Region2;
    DWORD Region2Size;
    if(SUCCEEDED(GlobalSecondaryBuffer->Lock(0, SoundOutput->SecondaryBufferSize,
                                             &Region1, &Region1Size,
                                             &Region2, &Region2Size,
                                             0)))
    {
        // TODO: Assert that region1size/region2size is valid
        uint8_t *DestSample = (uint8_t *)Region1;
        for(DWORD ByteIndex = 0; ByteIndex < Region1Size; ++ByteIndex)
        {
        	*DestSample++ = 0;
        }
        
        DestSample = (uint8_t *)Region2;
        for(DWORD ByteIndex = 0; ByteIndex < Region2Size; ++ByteIndex)
        {
        	*DestSample++ = 0;
        }
        
        GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
    }
}

internal void 
Win32FillSoundBuffer(win32_sound_output *SoundOutput, DWORD ByteToLock, DWORD BytesToWrite, game_sound_buffer *SourceBuffer)
{
    // TODO: More strenuous test!
    VOID *Region1;
    DWORD Region1Size;
    VOID *Region2;
    DWORD Region2Size;
    if(SUCCEEDED(GlobalSecondaryBuffer->Lock(ByteToLock, BytesToWrite,
                                             &Region1, &Region1Size,
                                             &Region2, &Region2Size,
                                             0)))
    {
        // TODO: assert that Region1Size/Region2Size are valid
		DWORD Region1SampleCount = Region1Size/(SoundOutput->BytesPerSample);
        int16_t *DestSample = (int16_t *)Region1;
        int16_t *SourceSample = SourceBuffer->Samples;
        
        for(DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex)
        {
        	*DestSample++ = *SourceSample++;
            *DestSample++ = *SourceSample++;
            ++SoundOutput->RunningSampleIndex;
        }
        
        DWORD Region2SampleCount = Region2Size/(SoundOutput->BytesPerSample);
        
        DestSample= (int16_t *)Region2;
    	for(DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex)
    	{
        	*DestSample++ = *SourceSample++;
            *DestSample++ = *SourceSample++;
            ++SoundOutput->RunningSampleIndex;
        }
        
    	GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
	}
}

// ------------------------  WINDOW RELATED ITEMS ------------------------

internal win32_window_dimension
Win32GetWindowDimension(HWND Window)
{
	win32_window_dimension WindowDimension;
    
	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	WindowDimension.Width = ClientRect.right - ClientRect.left;
	WindowDimension.Height = ClientRect.bottom - ClientRect.top;
	
	return(WindowDimension);
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
  	if(Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}
    
	Buffer->Width = Width;
	Buffer->Height = Height;
    
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width; // updated Width
	Buffer->Info.bmiHeader.biHeight = -(Buffer->Height); // updated Height (negative is top-down)
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression= BI_RGB;
    
	// 4 bytes per pixel, Width*Height = total pixels
	int BitmapMemorySize = BYTESPERPIXEL * (Buffer->Width * Buffer->Height);
	Buffer->Pitch = Width*BYTESPERPIXEL; // size of a single row in bytes
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
	// TODO: Probably clear this to black
}

// This is what we'll use to draw to the screen
internal void
Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer, HDC DeviceContext, 
                           int WindowWidth, int WindowHeight)
{
	/* This will slow us down a bit, but it will help eliminate gaps in lines
	 * that would otherwise appear (because StretchDIBits loses some information) */
	SetStretchBltMode(DeviceContext, HALFTONE);

	StretchDIBits(DeviceContext,
                  0, 0, WindowWidth, WindowHeight, 		// dest window
                  0, 0, Buffer->Width, Buffer->Height,	// src buffer
                  Buffer->Memory,
                  &(Buffer->Info),
                  DIB_RGB_COLORS, // we're using RGB colors in our buffer
                  SRCCOPY); // see Raster operators on msdn for other options
}

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory)
{
	if(Memory)
	{
		VirtualFree(Memory, 0, MEM_RELEASE);
	}
}

// TODO: Test file reading and writing!
DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile)
{
	debug_read_file_result ReadResult = {0};
	HANDLE FileHandle = CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(FileHandle != INVALID_HANDLE_VALUE)
	{
		void *Result = 0;
		LARGE_INTEGER FileSize;
		if(GetFileSizeEx(FileHandle, &FileSize))
		{
			uint32_t FileSize32 = SafeTruncateUInt64(FileSize.QuadPart);			
			Result = VirtualAlloc(0, FileSize32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			if(Result)
			{
				DWORD BytesRead;
				if(ReadFile(FileHandle, Result, FileSize32, &BytesRead, 0) && FileSize32 == BytesRead)
				{
					ReadResult.Content = Result;
					ReadResult.ContentSize = FileSize32;
				}
				else
				{
					DEBUGPlatformFreeFileMemory(Result);
					Result = 0;
				}
			}
			else
			{
				// TODO: Log the failure to allocate memory for file contents
			}
		}
		else
		{
			// TODO: Log failure getting the file size
		}
		CloseHandle(FileHandle);
	}
	else
	{
		// TODO: Log failure to "CreateFile"
	}
	return(ReadResult);
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile)
{
	return false;
}

struct win32_game_code
{
	HMODULE GameDLL;
	game_update_and_render* UpdateAndRender;
	bool32 IsValid;
};

inline FILETIME
Win32GetLastWriteTime(char *Filename)
{
	WIN32_FIND_DATAA FileData;
	FILETIME LastWrite = {};
	HANDLE FindHandle = FindFirstFileA(Filename, &FileData);
	if(FindHandle != INVALID_HANDLE_VALUE)
	{
		LastWrite = FileData.ftLastWriteTime;
		FindClose(FindHandle);
	}

	return LastWrite;
}

internal win32_game_code
Win32LoadGameCode(void)
{
	win32_game_code Result;
	Result.UpdateAndRender = GameUpdateAndRenderStub;

	if(CopyFile(OrigGameDLLPath, TempGameDLLPath, FALSE)==0)
	{
		ErrorContinue(TEXT("CopyFile game.dll->game_temp.dll"));
	}
	Result.GameDLL = LoadLibraryA(TempGameDLLPath);
	if(Result.GameDLL)
	{
		Result.UpdateAndRender = (game_update_and_render *)GetProcAddress(Result.GameDLL, "GameUpdateAndRender");
		Result.IsValid = (Result.UpdateAndRender && 1); // Using this so we can include a similar process for sound later
	}
	else
	{
		ErrorContinue(TEXT("LoadLibraryA game_temp.dll"));
	}
	
	if(!Result.IsValid)
	{
		Result.UpdateAndRender = GameUpdateAndRenderStub;
	}
	return Result;
}

internal void
Win32UnloadGameCode(win32_game_code *GameCodeDLL)
{
	if(GameCodeDLL->GameDLL)
	{
		if(FreeLibrary(GameCodeDLL->GameDLL) == 0)
		{
			ErrorContinue(TEXT("FreeLibrary"));
		}
		else
		{
			GameCodeDLL->GameDLL = 0;
			GameCodeDLL->UpdateAndRender = GameUpdateAndRenderStub;
			GameCodeDLL->IsValid = false;
		}
	}
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND 	Window,
						UINT 	Message,
						WPARAM 	WParam,
						LPARAM 	LParam)
{
	LRESULT Result = 0;
    
	// Here we'll handle the different window messages
	switch(Message){
        
		case WM_CLOSE:
		{
			// TODO: Handle this with a message to the user?
			GlobalRunning = false;
		} break;
        
		case WM_ACTIVATEAPP:
		{
			OutputDebugString("WM_ACTIVATEAPP\n");
		} break;
        
        // Handle keyboard input
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            Assert(!"KeyboardInput from outside dispatch");
        }
        
		case WM_DESTROY:
		{
			//TODO: Handle this as an error - maybe recreate window?
			GlobalRunning = false;
		} break;
        
		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);         
			win32_window_dimension Dimension = Win32GetWindowDimension(Window);
			Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext, 
                                       Dimension.Width, Dimension.Height);
			EndPaint(Window, &Paint);
		} break;
        
		default:
		{
			Result = DefWindowProcA(Window, Message, WParam, LParam);
		} break;
	} // end switch(Message0)
    
	return Result;
}

inline LARGE_INTEGER
Win32GetWallClock(void)
{
	LARGE_INTEGER ClockTime;
	QueryPerformanceCounter(&ClockTime);
	return(ClockTime);
}

inline real32
Win32GetElapsedTime(LARGE_INTEGER Start, LARGE_INTEGER End)
{
	real32 Result = ((real32)(End.QuadPart - Start.QuadPart)) / (real32)GlobalPerfCountFrequency;
	return(Result);
}

internal void
Win32DebugDrawVertical(win32_offscreen_buffer *Buffer, int x, int Top, int Bottom, uint32_t color)
{
	int y, end_y;
	int dy = Top - Bottom;
	//Swap the direction if necessary
	if(dy < 0) {y=Top; end_y=Bottom;}
	else {y=Bottom; end_y=Top;}

	// Clip the lines at the boundaries of the window height
	if(y < 0) {y = 0;}
	else if (y >= Buffer->Height) {y = (Buffer->Height - 1);}

	if(end_y < 0) {end_y = 0;}
	else if (end_y >= Buffer->Height) {end_y = (Buffer->Height - 1);}

	uint8_t *Row = (uint8_t *)Buffer->Memory + (y * Buffer->Pitch);
	Row = (uint8_t *)((uint32_t *)Row + x);
	for(; y < end_y; ++y)
	{
		*(uint32_t *)Row = color;
		Row += Buffer->Pitch;
	}
}

internal void
Win32DrawSoundMarker(win32_offscreen_buffer *Buffer, 
					DWORD Cursor, real32 C, int Top, int Bottom,
					win32_sound_output *SoundOutput, uint32_t color)
{
		int PadX = 16;
		Assert(Cursor < SoundOutput->SecondaryBufferSize);
		real32 XReal32 = C * Cursor;
		int X = PadX + (int)XReal32;
		Win32DebugDrawVertical(Buffer, X, Top, Bottom, color);
}

internal void
Win32DebugDisplaySync(win32_offscreen_buffer *Buffer, 
					uint32_t DebugSoundCursorArraySize, win32_debug_sound_cursors DebugSoundCursorArray[], 
					win32_sound_output *SoundOutput, real32 TargetSecondsPerFrame)
{
	int PadX = 16;
	int PadY = 16;
	int Bottom1 = (Buffer->Height)/2 - PadY;
	int Top1 = PadY;
	int Top2 = Bottom1 + PadY;
	int Bottom2 = Buffer->Height - PadY;

	real32 C = (real32)(Buffer->Width - 2*PadX) / (real32)SoundOutput->SecondaryBufferSize;

	for(uint32_t LastCursorIndex = 0; 
		LastCursorIndex < DebugSoundCursorArraySize; 
		LastCursorIndex++)
	{
		win32_debug_sound_cursors TheseCursors = DebugSoundCursorArray[LastCursorIndex];
		Win32DrawSoundMarker(Buffer, TheseCursors.PlayCursor, C, Top1, Bottom1, SoundOutput, 0xFFFFFFFF);
		Win32DrawSoundMarker(Buffer, TheseCursors.WriteCursor, C, Top2, Bottom2, SoundOutput, 0xFF0000FF);
	}
}

int CALLBACK
WinMain(HINSTANCE 	Instance,
		HINSTANCE 	PrevInstance,
		LPSTR 		CommandLine,
		int 		ShowCommand)
{
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    GlobalPerfCountFrequency = PerfCountFrequencyResult.QuadPart;
    real32 uSPerFrame = 0;
    
	// NOTE: Set the schedular granularity so our sleep works correctly
	UINT SchedulerGranularityMS = 1;
	bool32 GranularSleepOK = false;
	if(timeBeginPeriod(SchedulerGranularityMS) == TIMERR_NOERROR )
	{
		GranularSleepOK = true;
	}

	WNDCLASS WindowClass = {};
    
    Win32ResizeDIBSection(&GlobalBackBuffer, WIDTH, HEIGHT);
    
	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallback; // This is what will be called when there's a message 
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon;
	WindowClass.lpszClassName = "HandmadeGameWindowClass";
    
	// TODO: Obtain GameRefreshHz and MonitorRefreshHz dynamically
#define MonitorRefreshHz 60
#define GameRefreshHz (MonitorRefreshHz/2)
	real32 TargetSecondsPerFrame = (1.0f) / (real32)GameRefreshHz;

	if(RegisterClassA(&WindowClass))
	{
		HWND Window = CreateWindowExA(0,
                                      WindowClass.lpszClassName,
                                      "Game",
                                      WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      WIDTH, // used to be CW_USEDEFAULT
                                      HEIGHT, // used to be CW_USEDEFAULT
                                      0,
                                      0,
                                      Instance,
                                      0);
		if(Window)
		{
			// NOTE: Since we specified CS_OWNDC, we can just
			// get one device context and use it forever
			// because we're not sharing it with anyone
			HDC DeviceContext = GetDC(Window);
            
			GlobalRunning = true;
            
			win32_sound_output SoundOutput = {};
            
			// TODO: Make this longer? Perhaps 60 seconds?
			SoundOutput.SamplesPerSecond = 48000;
			SoundOutput.RunningSampleIndex = 0;
			SoundOutput.BytesPerSample = sizeof(int16_t)*2;
			SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond*SoundOutput.BytesPerSample;
			SoundOutput.LatencySampleCount = SoundOutput.SamplesPerSecond / 8;
			Win32InitDSound(Window, SoundOutput.SamplesPerSecond, SoundOutput.SecondaryBufferSize);
			Win32ClearBuffer(&SoundOutput);
			GlobalSecondaryBuffer->Play(0,0,DSBPLAY_LOOPING);
            
            // TODO: Pool with Bitmap VirtualAlloc
            int16_t *Samples = (int16_t *)VirtualAlloc(0, SoundOutput.SecondaryBufferSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);


            // NOTE: This just will help with debugging, we can
            // always expect our buffer to be at the same location
#if GAME_INTERNAL
            LPVOID BaseAddress = (LPVOID)Terabytes(2);
#else
            LPVOID BaseAddress = 0;
#endif
            // Allocate basically all the memory for our game
            game_memory GameMemory = {};
            GameMemory.PermanentStorageSize = Megabytes(64);
            GameMemory.TransientStorageSize = Gigabytes(1);
            
            // TODO: Check how much system memory is actually available, and allocate
            // based on that size
            uint64_t TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
            
            GameMemory.PermanentStorage = VirtualAlloc(BaseAddress,(size_t)TotalSize, 
                                                       MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            
            GameMemory.TransientStorage = ((uint8_t *)GameMemory.PermanentStorage + 
                                           GameMemory.PermanentStorageSize);
            
			if(Samples && GameMemory.PermanentStorage && GameMemory.TransientStorage)
			{
				LARGE_INTEGER LastCounter = Win32GetWallClock();
				uint64_t LastCycleCount = 0;

				MSG Message;

				uint32_t DebugSoundCursorIndex = 0;
				win32_debug_sound_cursors DebugSoundCursors[GameRefreshHz / 2] = {0};

				// Initialize our keyboard input - start with all keys in the up state
				game_keyboard_input Input = {};
				Input.num_keys = NumberOfDefinedKeys;
				for(uint32_t i=0; i<Input.num_keys; i++)
				{
					// Initialize the individual keys themselves
					Input.keys[i].prev_state_up = true;
					Input.keys[i].curr_state_up = true;
					Input.keys[i].num_presses = 0;

					// Update the released/pressed arrays
					// recall, these are just for our convenience
					// at the moment
					Input.keys_released[i] = false;
					Input.keys_pressed[i] = false;
				}

				// TODO: Initialize gamepad state (and combine gamepad and keyboard inputs)

				win32_game_code Game = Win32LoadGameCode();
				while(GlobalRunning){
					FILETIME OrigDLLWriteTime = Win32GetLastWriteTime(OrigGameDLLPath);
					FILETIME TempDLLWriteTime = Win32GetLastWriteTime(TempGameDLLPath);
					if(CompareFileTime(&OrigDLLWriteTime, &TempDLLWriteTime) > 0)
					{
						Win32UnloadGameCode(&Game);
						Game = Win32LoadGameCode();
					}

					while(PeekMessage(&Message, 0,0,0, PM_REMOVE))
					{
						if(Message.message == WM_QUIT)
						{
							GlobalRunning = false;
						}
						
						switch(Message.message)
						{
							// In order to handle and track keyboard input,
							// we'll maintain information about the key's that have been pressed
							// and how many times they have been pressed since the last frame.
							// We also want to keep track of whether or not the key was left in the "up"
							// or "down" state.
							// And we want to be sure we can keep track of whether keys have been
							// held down vs. press & release.	                	

							// Handle keyboard input - for now, we won't deal with system keys
							//case WM_SYSKEYDOWN:
							//case WM_SYSKEYUP:
							VKEY key;

							case WM_KEYDOWN:
							case WM_KEYUP:
							{
								switch(Message.wParam)
								{	
									// Handle different types of Control Keystrokes
									case VK_ESCAPE: {key = ESC;} break;
									case VK_TAB: {key = TAB;} break;
									case VK_CAPITAL: {key = CAPS;} break;
									case VK_LSHIFT: {key = LSHIFT;} break;
									case VK_LCONTROL: {key = LCTRL;} break;
									case VK_LMENU: {key = LALT;} break;
									case VK_SPACE: {key = SPACE;} break;
									case VK_RMENU: {key = RALT;} break;
									case VK_RCONTROL: {key = RCTRL;} break;
									case VK_RSHIFT: {key = RSHIFT;} break;
									case VK_RETURN: {key = ENTER;} break;
									case VK_BACK: {key = BACKSPACE;} break;
									case VK_LEFT: {key = LEFT;} break;
									case VK_DOWN: {key = DOWN;} break;
									case VK_RIGHT: {key = RIGHT;} break;
									case VK_UP: {key = UP;} break;
	
									// Handle Numeric Characters
									case '0': {key = ZERO;} break;
									case '1': {key = ONE;} break;
									case '2': {key = TWO;} break;
									case '3': {key = THREE;} break;
									case '4': {key = FOUR;} break;
									case '5': {key = FIVE;} break;
									case '6': {key = SIX;} break;
									case '7': {key = SEVEN;} break;
									case '8': {key = EIGHT;} break;
									case '9': {key = NINE;} break;

									// Handle Alpha Characters
									case 'Q': {key = Q;} break;
									case 'W': {key = W;} break;
									case 'E': {key = E;} break;
									case 'R': {key = R;} break;
									case 'T': {key = T;} break;
									case 'Y': {key = Y;} break;
									case 'U': {key = U;} break;
									case 'I': {key = I;} break;
									case 'O': {key = O;} break;
									case 'P': {key = P;} break;
									case 'A': {key = A;} break;
									case 'S': {key = S;} break;
									case 'D': {key = D;} break;
									case 'F': {key = F;} break;
									case 'G': {key = G;} break;
									case 'H': {key = H;} break;
									case 'J': {key = J;} break;
									case 'K': {key = K;} break;
									case 'L': {key = L;} break;
									case 'Z': {key = Z;} break;
									case 'X': {key = X;} break;
									case 'C': {key = C;} break;
									case 'V': {key = V;} break;
									case 'B': {key = B;} break;
									case 'N': {key = N;} break;
									case 'M': {key = M;} break;
								}	

								if(Message.message == WM_KEYDOWN)
								{
									Input.keys_pressed[key] = true;
									// TODO - test and fix counting the number of presses
									// I think we might be able to get this from the WM_KEYDOWN message already
									//if(Input.keys[key].prev_state_up) { Input.keys[key].num_presses++; }
									Input.keys[key].prev_state_up = Input.keys[key].curr_state_up;
									Input.keys[key].curr_state_up = false;									
								}
								else
								{
									Input.keys_released[key] = true; 
									Input.keys[key].prev_state_up = Input.keys[key].curr_state_up;
									Input.keys[key].curr_state_up = true;
								}	                    	                    			                    		                
							} break;

							default:
							{
								TranslateMessage(&Message);
								DispatchMessage(&Message);
							}
						} // end switch(Message.message)
					} // end while PeekMessage 
				
					// TODO: Make sure this is guarded entirely - we'll be changing in the future anyway
                    DWORD ByteToLock = 0;
                    DWORD TargetCursor = 0;
                    DWORD BytesToWrite = 0;
                    DWORD PlayCursor = 0;
                    DWORD WriteCursor = 0;
                    bool32 SoundIsValid = false;
                    // TODO: Tighten up sound logic so we know where we'll be writing to
                    // and can anticipate the time spent in the game update
                    if(SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
                    {
                        ByteToLock = (SoundOutput.RunningSampleIndex * SoundOutput.BytesPerSample) % SoundOutput.SecondaryBufferSize;
                        
                        // In order to handle improved latency, we only want to write
                        // LatencySampleCount * BytesPerSample bytes past play cursor so we
                        // can make changes more quickly
                        TargetCursor = (PlayCursor + (SoundOutput.LatencySampleCount * SoundOutput.BytesPerSample)) % SoundOutput.SecondaryBufferSize;
                        
                        if (ByteToLock > TargetCursor)
                        {
                            BytesToWrite = SoundOutput.SecondaryBufferSize - ByteToLock;
                            BytesToWrite += TargetCursor;
                        }
                        else
                        {
                            BytesToWrite = TargetCursor - ByteToLock;
                        }
                        SoundIsValid = true;
                    }
                    
                    // Working with 30FPS for now
                    // TODO: Make this NOT jank
                    game_sound_buffer SoundBuffer = {};
                    SoundBuffer.SamplesPerSecond = SoundOutput.SamplesPerSecond;;
                    SoundBuffer.SampleCount = BytesToWrite / SoundOutput.BytesPerSample;
                    SoundBuffer.Samples = Samples;

					game_offscreen_buffer Buffer = {};
					Buffer.Memory = GlobalBackBuffer.Memory;
					Buffer.Width = GlobalBackBuffer.Width;
					Buffer.Height = GlobalBackBuffer.Height;
					Buffer.Pitch = GlobalBackBuffer.Pitch;

					/* CALL FROM PLATFORM LAYER TO OUR GAME 
					* THIS WILL POPULATE THE BACKBUFFER SO
					* WE KNOW WHAT TO DRAW */
					Game.UpdateAndRender(&GameMemory, &Buffer, uSPerFrame, &Input, &SoundBuffer);

                    if(SoundIsValid)
                    {
                        // Call buffer setup code
                        Win32FillSoundBuffer(&SoundOutput, ByteToLock, BytesToWrite, &SoundBuffer);
                    }

					// NOTE: Timing-related code to keep track of our FPS & microseconds per frame
					// we can and will be extended this in the future to build out our profiler
					LARGE_INTEGER EndCounter = Win32GetWallClock();
					real32 TrackingSecondsElapsed = Win32GetElapsedTime(LastCounter, EndCounter);
					if(TrackingSecondsElapsed < TargetSecondsPerFrame)
					{
						if(GranularSleepOK)
						{
							// NOTE: Subtracting off 1.5 here is a hack to make up for the fact that
							// we were unable to meet our frame rate before
							DWORD SleepMS = (DWORD)((1000.0f * (TargetSecondsPerFrame - TrackingSecondsElapsed))-1.5);
							if(SleepMS > 0)
							{
								Sleep(SleepMS);
							}	
							TrackingSecondsElapsed = Win32GetElapsedTime(LastCounter, Win32GetWallClock());
							Assert(TrackingSecondsElapsed < TargetSecondsPerFrame);
						}

						while(TrackingSecondsElapsed < TargetSecondsPerFrame)
						{
							TrackingSecondsElapsed = Win32GetElapsedTime(LastCounter, Win32GetWallClock());
						}
					}
					else{ // TODO: Handled missed framerate
						// TODO: properly log here
						OutputDebugString("Missed frame rate.\n");
					}

					EndCounter = Win32GetWallClock();
					// TOOD: might modify these to floating point to get some fractional precision
					real32 msPerFrame = (1000.0f * Win32GetElapsedTime(LastCounter, EndCounter));
					uSPerFrame = 1000.0f * msPerFrame; // Used by game-update and render!
					LastCounter = EndCounter;

						/* Now that we've waited the frame time, display next frame */
					win32_window_dimension Dimension = Win32GetWindowDimension(Window);
#if GAME_INTERNAL
					Win32DebugDisplaySync(&GlobalBackBuffer, 
										ArrayCount(DebugSoundCursors), DebugSoundCursors, 
										&SoundOutput, TargetSecondsPerFrame);
#endif
					// Display the backbuffer in the window
					Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext, 
											Dimension.Width, Dimension.Height);
					ReleaseDC(Window, DeviceContext);

#if GAME_INTERNAL
					{
						win32_debug_sound_cursors SoundCursors;
						GlobalSecondaryBuffer->GetCurrentPosition(&(SoundCursors.PlayCursor), &(SoundCursors.WriteCursor));
						DebugSoundCursors[(DebugSoundCursorIndex++)%(ArrayCount(DebugSoundCursors))] = SoundCursors;
					}
#endif

					uint64_t EndCycleCount = __rdtsc();
					uint64_t CyclesElapsed = (EndCycleCount - LastCycleCount);
					LastCycleCount = EndCycleCount; 	 

#if GAME_INTERNAL				
					int32_t FPS = (int32_t)(GlobalPerfCountFrequency / uSPerFrame);
					int32_t MCPF = (int32_t)(CyclesElapsed/ (1000*1000));
					
					// TODO: clean up this code because of the inherent peril - NEVER SHIP
					char PrintBuffer[256];
					sprintf_s(PrintBuffer, "%.3fms/f | %df/s | %dMcycles/f\n", msPerFrame, FPS, MCPF);
					OutputDebugString(PrintBuffer);
#endif

				} // end while GlobalRunning
			} // end memory check
    	}
		else
		{
			OutputDebugString("FATAL ERROR: Failed to create window handle.");
		    // TODO: log the failed window handle
		}
	}
	else
	{
		OutputDebugString("FATAL ERROR: Failed to WindowClass Registration.");
	}
}