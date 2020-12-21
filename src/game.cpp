#include "game.h"

// Temporarily using these globals, will find a better system later
global_variable Mesh cube;
global_variable real32 projMatrix[4][4] = {0};
global_variable real32 fTheta = 1.0f;

// this is stop-gap hack
// TODO: Move this functionality to a proper
// initialization system
bool32 initialized = false;

internal void
Init(game_memory *Memory, game_offscreen_buffer *Buffer)
{
	// Set up a basic cube
	// TODO: we probably should be handling this elsewhere
	// this is just set up to get it working now
	cube.tris = {
		// SOUTH
		{{0.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f}, {1.0f,1.0f,0.0f}},
		{{0.0f,0.0f,0.0f}, {1.0f,1.0f,0.0f}, {1.0f,0.0f,0.0f}},

		// EAST
		{{1.0f,0.0f,0.0f}, {1.0f,1.0f,.0f}, {1.0f,1.0f,1.0f}},
		{{1.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f}, {1.0f,0.0f,1.0f}},

		// NORTH
		{{1.0f,0.0f,1.0f}, {1.0f,1.0f,1.0f}, {0.0f,1.0f,1.0f}},
		{{1.0f,0.0f,1.0f}, {0.0f,1.0f,1.0f}, {1.0f,0.0f,1.0f}},

		// WEST
		{{0.0f,0.0f,1.0f}, {0.0f,1.0f,1.0f}, {0.0f,1.0f,0.0f}},
		{{0.0f,0.0f,1.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f,0.0f}},

		// TOP
		{{0.0f,1.0f,0.0f}, {0.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}},
		{{0.0f,1.0f,0.0f}, {1.0f,1.0f,1.0f}, {1.0f,1.0f,0.0f}},

		// BOTTOM
		{{1.0f,0.0f,1.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f,0.0f}},
		{{1.0f,0.0f,1.0f}, {0.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f}},
	};

	// Projection Matrix
	real32 fNear = 0.1f;
	real32 fFar = 10000.0f;
	real32 fFov = 90.0f;
	real32 fAspectRatio = (real32)Buffer->Height / (real32)Buffer->Width;
	real32 fFovRad = 1.0f / tanf((real32)(fFov * 0.5f) / (real32)(180.0f * Pi32));

	projMatrix[0][0] = fAspectRatio * fFovRad;
	projMatrix[1][1] = fFovRad;
	projMatrix[2][2] = fFar / (fFar - fNear);
	projMatrix[3][2] = (-fFar * fNear) / (fFar - fNear);
	projMatrix[2][3] = 1.0f;
	projMatrix[3][3] = 0.0f;

	initialized = true;
}

internal void 
GameUpdateAndRender(game_memory *Memory, game_offscreen_buffer *Buffer, real32 delta_time)
{
	// We should do some better initialization
	if(!initialized) {Init(Memory, Buffer);}

	// These are just some test drawings to make sure our drawing functionality works!
	FillColor(Buffer, BLACK);

#if 0
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
#endif

	// Set up rotation matrices
	real32 matRotZ[4][4] = {0};
	real32 matRotX[4][4] = {0};
	fTheta += 1.0f * (delta_time / (1000 * 1000));

	// Rotation Z
	matRotZ[0][0] = cosf(fTheta);
	matRotZ[0][1] = sinf(fTheta);
	matRotZ[1][0] = -sinf(fTheta);
	matRotZ[1][1] = cosf(fTheta);
	matRotZ[2][2] = 1;
	matRotZ[3][3] = 1;

	// Rotation X
	matRotX[0][0] = 1;
	matRotX[1][1] = cosf(fTheta * 0.5f);
	matRotX[1][2] = sinf(fTheta * 0.5f);
	matRotX[2][1] = -sinf(fTheta * 0.5f);
	matRotX[2][2] = cosf(fTheta * 0.5f);
	matRotX[3][3] = 1;

	for (Triangle_3D tri : cube.tris)
	{
		Triangle_3D triTranslated, triProjected, triRotatedZ, triRotatedZX;
		Triangle_2D triProjected_2D = {0};

		// Rotate in Z-Axis
		MatrixVecMult(&triRotatedZ.p1, &tri.p1, matRotZ);
		MatrixVecMult(&triRotatedZ.p2, &tri.p2, matRotZ);
		MatrixVecMult(&triRotatedZ.p3, &tri.p3, matRotZ);

		// Rotate in X-Axis
		MatrixVecMult(&triRotatedZX.p1, &triRotatedZ.p1, matRotX);
		MatrixVecMult(&triRotatedZX.p2, &triRotatedZ.p2, matRotX);
		MatrixVecMult(&triRotatedZX.p3, &triRotatedZ.p3, matRotX);

		// Offset into the screen
		triTranslated = triRotatedZX;
		triTranslated.p1.z = triRotatedZX.p1.z + 100.0f;
		triTranslated.p2.z = triRotatedZX.p2.z + 100.0f;
		triTranslated.p3.z = triRotatedZX.p3.z + 100.0f;
	
	/*	
		triTranslated = tri;
		triTranslated.p1.z = tri.p1.z + 3.0f;
		triTranslated.p2.z = tri.p2.z + 3.0f;
		triTranslated.p3.z = tri.p3.z + 3.0f;
	*/

		// Project triangles from 3D --> 2D
		MatrixVecMult(&triProjected.p1, &triTranslated.p1, projMatrix);
		MatrixVecMult(&triProjected.p2, &triTranslated.p2, projMatrix);
		MatrixVecMult(&triProjected.p3, &triTranslated.p3, projMatrix);

		// Scale into view - and eliminate the z component
		triProjected_2D.p1.x = triProjected.p1.x + 1.f; triProjected_2D.p1.y = triProjected.p1.y + 1.f;
		triProjected_2D.p2.x = triProjected.p2.x + 1.f; triProjected_2D.p2.y = triProjected.p2.y + 1.f;
		triProjected_2D.p3.x = triProjected.p3.x + 1.f; triProjected_2D.p3.y = triProjected.p3.y + 1.f;
		triProjected_2D.p1.x *= (0.5f * (real32)Buffer->Width);
		triProjected_2D.p1.y *= (0.5f * (real32)Buffer->Height);
		triProjected_2D.p2.x *= (0.5f * (real32)Buffer->Width);
		triProjected_2D.p2.y *= (0.5f * (real32)Buffer->Height);
		triProjected_2D.p3.x *= (0.5f * (real32)Buffer->Width);
		triProjected_2D.p3.y *= (0.5f * (real32)Buffer->Height);

		// Rasterize triangle
		DrawTriangle_2D(Buffer, triProjected_2D.p1, triProjected_2D.p2, triProjected_2D.p3, WHITE);
	}

	// This is called 'per-frame'
}