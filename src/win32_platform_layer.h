/* date = August 14th 2020 0:19 pm */

#ifndef WIN32_GAME_H
#define WIN32_GAME_H

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
