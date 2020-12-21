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

//#if 0
	for (Triangle_3D tri : cube.tris)
	{
		Triangle_2D translatedTri = {{tri.p1.x,tri.p1.y}, {tri.p2.x, tri.p2.y}, {tri.p3.x, tri.p3.y}};
		DrawTriangle_2D(Buffer, translatedTri.p1, translatedTri.p2, translatedTri.p3, WHITE);
	}
//#endif

#if 0
	Point_2D a, b, c;
	a.x = 0; a.y = 0;
	b.x = 0; b.y = 100;
	c.x = 100; c.y = 100;
	DrawTriangle_2D(Buffer, a, b, c, WHITE);

	Point_2D d,e;
	d.x = 800; d.y=400;
	e.x = 900; e.y=500;
	FillRect_2D(Buffer, d,e, BLUE);

	Point_2D f,g;
	f.x = 300; f.y=50;
	g.x = 500; g.y=150;
	FillRect_2D(Buffer, f, g, GREEN);
#endif

	// This is called 'per-frame'
}