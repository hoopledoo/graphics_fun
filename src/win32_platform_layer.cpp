// Following along with the Handmade youtube series
// Author: Tyler Wolf

// Need to get the types at the very beginning
#include <stdint.h>
// TODO: Implement sine ourselves?
#include <math.h>

#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.141592653589793238462643383279

typedef float real32;
typedef double real64;
typedef char bool32;

// Remember - we're just building one giant file (translation unit)
// rather than splitting out into tons of tiny pieces
// this might be a controversial technique...
// ------------------------------------------------------------------
// NOTE: drawing.cpp requires the #defines and typedefs above
#include "drawing.cpp"
// NOTE: game.cpp requires the #defines and typedefs above ^
#include "game.cpp"
// ------------------------------------------------------------------

#include <malloc.h>
#include <windows.h>

#include "win32_platform_layer.h"

// TODO: This is a global for now, probably should change later.
global_variable bool32 GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackBuffer;

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

internal debug_read_file_result 
DEBUGPlatformReadEntireFile(char *Filename)
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

internal bool32 
DEBUGPlatformWriteEntireFile(char *Filename, uint32_t MemorySize, void *Memory)
{

}

internal void 
DEBUGPlatformFreeFileMemory(void *Memory)
{
	if(Memory)
	{
		VirtualFree(Memory, 0, MEM_RELEASE);
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

int CALLBACK
WinMain(HINSTANCE 	Instance,
		HINSTANCE 	PrevInstance,
		LPSTR 		CommandLine,
		int 		ShowCommand)
{
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    int64_t PerfCountFrequency = PerfCountFrequencyResult.QuadPart;
    int64_t LastCycleCount = 0;
    int32_t uSPerFrame = 0;

    LARGE_INTEGER LastCounter;
    QueryPerformanceCounter(&LastCounter);
    
	WNDCLASS WindowClass = {};
    
    Win32ResizeDIBSection(&GlobalBackBuffer, WIDTH, HEIGHT);
    
	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallback; // This is what will be called when there's a message 
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon;
	WindowClass.lpszClassName = "HandmadeGameWindowClass";
    
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
            MSG Message;

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

            while(GlobalRunning){
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
            
	            game_offscreen_buffer Buffer = {};
	            Buffer.Memory = GlobalBackBuffer.Memory;
	            Buffer.Width = GlobalBackBuffer.Width;
	            Buffer.Height = GlobalBackBuffer.Height;
	            Buffer.Pitch = GlobalBackBuffer.Pitch;

	            /* CALL FROM PLATFORM LAYER TO OUR GAME 
	             * THIS WILL POPULATE THE BACKBUFFER SO
	             * WE KNOW WHAT TO DRAW */
	            GameUpdateAndRender(&GameMemory, &Buffer, (real32)uSPerFrame, &Input);
            
	            /* We've returned from the game code */
	            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
	            // Display the backbuffer in the window
	            Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext, 
	                                       Dimension.Width, Dimension.Height);
	            ReleaseDC(Window, DeviceContext);
	            
	            int64_t EndCycleCount = __rdtsc();
	            
	            // NOTE: Timing-related code to keep track of our FPS & microseconds per frame
	            // we can and will be extended this in the future to build out our profiler
	            LARGE_INTEGER EndCounter;
	            QueryPerformanceCounter(&EndCounter);
	            
	            // Get the differences
	            int64_t CyclesElapsed = (EndCycleCount - LastCycleCount);
	            int64_t CounterElapsed = (EndCounter.QuadPart - LastCounter.QuadPart);
	            
	            // TOOD: might modify these to floating point to get some fractional precision
	            uSPerFrame = (int32_t)((1000*1000*CounterElapsed) / PerfCountFrequency);
	            int32_t FPS = (int32_t)(PerfCountFrequency / CounterElapsed);
	            int32_t MCPF = (int32_t)(CyclesElapsed/ (1000*1000));
	            
	#if 0
	            // TODO: clean up this code because of the inherent peril - NEVER SHIP
	            char PrintBuffer[256];
	            wsprintf(PrintBuffer, "%dms/f | %df/s | %dMcycles/f\n", uSPerFrame, FPS, MCPF);
	            OutputDebugString(PrintBuffer);
	#endif
	            
	            LastCounter = EndCounter;
	            LastCycleCount = EndCycleCount; 	         
        	} // end while GlobalRunning
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