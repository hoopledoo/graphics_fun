#include "game.h"

internal void 
GameUpdateAndRender(game_memory *Memory, game_offscreen_buffer *Buffer)
{
	FillColor(Buffer, BLACK);

	Point_2D a, b, c;
	a.x = 0; a.y = 0;
	b.x = 400; b.y = 100;
	c.x = 100; c.y = 400;
	DrawTriangle_2D(Buffer, a, b, c, WHITE);

	Point_2D d,e;
	d.x = 800; d.y=400;
	e.x = 900; e.y=500;
	FillRect_2D(Buffer, d,e, BLUE);
	// This is called 'per-frame'
}