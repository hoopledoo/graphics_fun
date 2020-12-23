/* date = August 14th 2020 0:19 pm */

#ifndef WIN32_GAME_H
#define WIN32_GAME_H

// TODO: We may want to handle this differently later
#define BYTESPERPIXEL 4

// This is temporary, TODO: we want to better handle window sizing later
#define WIDTH 1280
#define HEIGHT 720

struct win32_offscreen_buffer
{
	BITMAPINFO Info;
	void* Memory;
	int Width;
	int Height;
	int Pitch;
};

struct win32_window_dimension
{
	int Width;
	int Height;
};

#endif //WIN32_GAME_H
