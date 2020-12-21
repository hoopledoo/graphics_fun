#include "game.h"

// Temporarily using these globals
global_variable Mesh cube;

// this is stop-gap hack
// TODO: Move this functionality to a proper
// initialization system
bool32 initialized = false;

internal void
Init()
{
	// Set up a basic cube
	// TODO: we probably should be handling this elsewhere
	// this is just set up to get it working now
	cube.tris = {
		// NORTH
		{{0,0,100},{0,100,100},{100,100,100}},
		{{0,0,100},{100,100,100},{100,0,100}},

		// EAST
		{{0,0,0},{0,0,100},{0,100,100}},
		{{0,0,0},{0,100,100},{0,100,0}},

		// SOUTH
		{{0,0,0},{0,100,0},{100,100,0}},
		{{0,0,0},{100,100,0},{100,0,0}},

		// WEST
		{{100,0,0},{100,0,100},{100,100,100}},
		{{100,0,0},{100,100,100},{100,100,0}},

		// TOP
		{{0,0,100},{0,100,100},{100,100,100}},
		{{0,0,100},{100,100,100},{100,0,100}},

		// BOTTOM
		{{0,0,0},{0,100,0},{100,100,0}},
		{{0,0,0},{100,100,0},{100,0,0}},
	};
	initialized = true;
}

internal void 
GameUpdateAndRender(game_memory *Memory, game_offscreen_buffer *Buffer)
{
	// We should do some better initialization
	if(!initialized) {Init();}

	// These are just some test drawings to make sure our drawing functionality works!
	FillColor(Buffer, BLACK);

#if 0
	for (Triangle_3D tri : cube.tris)
	{
		DrawTriangle_3D(Buffer, tri.p1, tri.p2, tri.p3, WHITE);
	}
#endif

//#if 0
	Point_2D a, b, c;
	a.x = 0.0f; a.y = 0.0f;
	b.x = 0.0f; b.y = 100.0f;
	c.x = 100.0f; c.y = 100.0f;
	DrawTriangle_2D(Buffer, a, b, c, WHITE);

	Point_2D d,e;
	d.x = 800.0f; d.y=400.0f;
	e.x = 900.0f; e.y=500.0f;
	FillRect_2D(Buffer, d,e, BLUE);

	Point_2D f,g;
	f.x = 300.0f; f.y=50.0f;
	g.x = 500.0f; g.y=150.0f;
	FillRect_2D(Buffer, f, g, GREEN);
//#endif

	// This is called 'per-frame'
}