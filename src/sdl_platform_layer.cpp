#include "sdl_platform_layer.h"

bool32 HandleEvent(SDL_Event *Event)
{
	bool32 ShouldQuit = false;

	switch(Event->type)
	{
		case SDL_WINDOWEVENT:
		{
			switch(Event->window.event)
			{
				case SDL_WINDOWEVENT_RESIZED:
				{
					printf("SDL_WINDOWEVENT_RESIZED (%d, %d)\n",
						Event->window.data1, Event->window.data2);
				} break;
			}	
		} break;
		case SDL_QUIT:
		{
			printf("SDL_QUIT\n");
			ShouldQuit = true;
		} break;	
	}
	fflush(stdout);
	return(ShouldQuit);
}

// This will be the equivalent of WinMain (probably)
int main(int argc, char *argv[])
{
	SDL_Window *GameWindow;
	linux_window_dimension WindowDimension;
	WindowDimension.Width = WIDTH;
	WindowDimension.Height = HEIGHT;
	// Initialize SDL - for now, only the VIDEO component of it
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{	
		//TODO: SDL_Init didn't work!
	}
	GameWindow = SDL_CreateWindow("Game",
								  SDL_WINDOWPOS_UNDEFINED,
								  SDL_WINDOWPOS_UNDEFINED,
								  WindowDimension.Width, 
								  WindowDimension.Height,
								  SDL_WINDOW_RESIZABLE);	

	for(;;)
	{
		SDL_Event Event = {};
		SDL_PollEvent(&Event);
		if(HandleEvent(&Event)) break;
	}

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Handmade Hero", "This is Handmade Hero", 0);	
	return 0;	
}
