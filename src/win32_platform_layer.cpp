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
#define BYTESPERPIXEL 4

typedef float real32;
typedef double real64;
typedef int32_t bool32;

// Remember - we're just building one giant file (translation unit)
// rather than splitting out into tons of tiny pieces
// this might be a controversial technique...
// NOTE: game.cpp requires the #defines and typedefs above ^
#include "game.cpp"

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
	StretchDIBits(DeviceContext,
                  0, 0, WindowWidth, WindowHeight, 		// dest window
                  0, 0, Buffer->Width, Buffer->Height,	// src buffer
                  Buffer->Memory,
                  &(Buffer->Info),
                  DIB_RGB_COLORS, // we're using RGB colors in our buffer
                  SRCCOPY); // see Raster operators on msdn for other options
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
    LARGE_INTEGER LastCounter;
    QueryPerformanceCounter(&LastCounter);
    
	WNDCLASS WindowClass = {};
    
    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);
    
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
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
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
            while(GlobalRunning){
	            while(PeekMessage(&Message, 0,0,0, PM_REMOVE))
	            {
	                if(Message.message == WM_QUIT)
	                {
	                    GlobalRunning = false;
	                }
	                
	                switch(Message.message)
	                {
	                    // Handle keyboard input
	                    case WM_SYSKEYDOWN:
	                    case WM_SYSKEYUP:
	                    case WM_KEYDOWN:
	                    case WM_KEYUP:
	                    {
	                    	uint32_t VKCode = (uint32_t)Message.wParam;
				        	if(VKCode=='Q')
				        	{
				        		GlobalRunning = false;
				        	}
	                        // Handle different keypresses.
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
	            GameUpdateAndRender(&GameMemory, &Buffer);
            
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
	            int32_t uSPerFrame = (int32_t)((1000*1000*CounterElapsed) / PerfCountFrequency);
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