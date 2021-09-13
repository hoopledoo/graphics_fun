/* date = Sept 14th 2021 5:24 pm */

#ifndef LINUX_GAME_H
#define LINUX_GAME_H

// TODO: We may want to handle this differently later
#define BYTESPERPIXEL 4

// This is temporary, TODO: we want to better handle window sizing later
#define WIDTH 1280
#define HEIGHT 720

#include <SDL.h>

// TODO - bring back in the game header file
#include "game.h"

struct linux_offscreen_buffer
{
	void* Memory;
	uint32_t Width;
	uint32_t Height;
	uint32_t Pitch;
};

struct linux_window_dimension
{
	uint32_t Width;
	uint32_t Height;
};

#endif //LINUX_GAME_H
