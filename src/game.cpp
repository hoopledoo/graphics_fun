#include "game.h"

// Temporarily using these globals, will find a better system later
global_variable Mesh cube;
global_variable real32 projMatrix[4][4] = {0};
global_variable real32 fTheta = 1.0f;
global_variable Vec3D vCamera = {0};
global_variable bool32 GlobalRotating = true;

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
		{{1.0f,0.0f,0.0f}, {1.0f,1.0f,0.0f}, {1.0f,1.0f,1.0f}},
		{{1.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f}, {1.0f,0.0f,1.0f}},

		// NORTH
		{{1.0f,0.0f,1.0f}, {1.0f,1.0f,1.0f}, {0.0f,1.0f,1.0f}},
		{{1.0f,0.0f,1.0f}, {0.0f,1.0f,1.0f}, {0.0f,0.0f,1.0f}},

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

	cube.offset.z = 50.0f;

	// Projection Matrix -- This is a placeholder until we implement a proper camera
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
DrawBoxes(game_offscreen_buffer *Buffer)
{
	Point_2D_Real a, b, c;
	a.x = 0.0f; a.y = 0.0f;
	b.x = 0.0f; b.y = 100.0f;
	c.x = 100.0f; c.y = 100.0f;
	DrawTriangle_2D(Buffer, a, b, c, WHITE);

	Point_2D_Real d,e;
	d.x = 0.0f; d.y=25.0f;
	e.x = 25.0f; e.y=50.0f;
	FillRect_2D(Buffer, d,e, 0xffffff);

	Point_2D_Real f,g;
	f.x = 35.0f; f.y=25.0f;
	g.x = 60.0f; g.y=50.0f;
	FillRect_2D(Buffer, f, g, 0xefefef);

	Point_2D_Real h,i;
	h.x = 70.0f; h.y=25.0f;
	i.x = 95.0f; i.y=50.0f;
	FillRect_2D(Buffer, h, i, 0xdfdfdf);

	Point_2D_Real j,k;
	j.x = 105.0f; j.y=25.0f;
	k.x = 130.0f; k.y=50.0f;
	FillRect_2D(Buffer, j, k, 0xcfcfcf);

	Point_2D_Real l,m;
	l.x = 140.0f; l.y=25.0f;
	m.x = 165.0f; m.y=50.0f;
	FillRect_2D(Buffer, l, m, 0xbfbfbf);

	Point_2D_Real n,o;
	n.x = 175.0f; n.y=25.0f;
	o.x = 200.0f; o.y=50.0f;
	FillRect_2D(Buffer, n, o, 0xafafaf);

	// To create darker shades of gray, we just need to multiple 0xff by some scale, 
	// and then shift and OR them together	
}

internal void
DrawTriangles(game_offscreen_buffer *Buffer)
{
	// Code that tests out filling individual triangles
	// Here. we've identified an issue where the ordering of the points matters
	// although it really shouldn't!
	Point_2D_Real p1,p2,p3;
	p1.x = 25; p1.y = 0;
	p2.x = 0; p2.y = 15;
	p3.x = 10; p3.y = 30;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);

	p1.x = 30; p1.y = 0;
	p2.x = 55; p2.y = 11;
	p3.x = 20; p3.y = 25;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);

	p1.x = 60; p1.y = 0;
	p2.x = 100; p2.y = 3;
	p3.x = 70; p3.y = 25;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);


	p1.x = 0; p1.y = 35;
	p2.x = 25; p2.y = 40;
	p3.x = 10; p3.y = 55;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);

	p1.x = 700.06f; p1.y = 35.213f + 15.6f*sinf(fTheta);
	p2.x = 800.231f; p2.y = 65.06f + 51.6f*sinf(fTheta);
	p3.x = 750.2931f; p3.y = 95.23959f + 35.6f*sinf(fTheta);
	FillTriangle_2D(Buffer, p1, p2, p3, YELLOW);

	p1.x = 200+50*cosf(fTheta); p1.y = 0;
	p2.x = 225 + 100*cosf(fTheta); p2.y = 100.83657362f;
	p3.x = 245; p3.y = 155+100*sinf(fTheta);
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);

	p1.x = 200+150*cosf(fTheta); p1.y = 400+150*sinf(fTheta);
	p2.x = 150; p2.y = 305;
	p3.x = 290; p3.y = 350;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);

	p1.x = 500; p1.y = 100;
	p2.x = 551/*+100*cosf(fTheta)*/; p2.y = 250+150*sinf(fTheta);
	p3.x = 501+100*cosf(fTheta); p3.y = 350;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);	
}

internal void
DrawCube(game_offscreen_buffer *Buffer, uint32_t base_color)
{
	// Set up rotation matrices
	real32 matRotZ[4][4] = {0};
	real32 matRotX[4][4] = {0};

	// These rotation matrices are place-holders to help demonstrate
	// 3D functionality, before we've implemented the camera

	// TODO: wrap up rotation matrix generation cleanly into our drawing.cpp "library"
	// These matrices are easy to derive, and we can look them up on wikipedia as well

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
		Triangle_3D triTranslated, triRotatedZ, triRotatedZX;
		real32 light_dot_product;

		// Rotate in Z-Axis
		MatrixVecMult(&triRotatedZ.p1, &tri.p1, matRotZ);
		MatrixVecMult(&triRotatedZ.p2, &tri.p2, matRotZ);
		MatrixVecMult(&triRotatedZ.p3, &tri.p3, matRotZ);

		// Rotate in X-Axis
		MatrixVecMult(&triRotatedZX.p1, &triRotatedZ.p1, matRotX);
		MatrixVecMult(&triRotatedZX.p2, &triRotatedZ.p2, matRotX);
		MatrixVecMult(&triRotatedZX.p3, &triRotatedZ.p3, matRotX);

		// Offset into the screen
		// TODO: this is a placeholder until a proper camera is implemented
		triTranslated = triRotatedZX;
		triTranslated.p1.z = triRotatedZX.p1.z + cube.offset.z;
		triTranslated.p2.z = triRotatedZX.p2.z + cube.offset.z;
		triTranslated.p3.z = triRotatedZX.p3.z + cube.offset.z;

		// TODO: this could probably be moved into DrawTriangle_3D
		// and is mostly a placeholder until a proper camera is implemented
		// Take the cross product to find the normal vector 
		// then dot-product the normal vector & the camera's
		Vec3D normal, line1, line2;

		line1.x = triTranslated.p2.x - triTranslated.p1.x;
		line1.y = triTranslated.p2.y - triTranslated.p1.y;
		line1.z = triTranslated.p2.z - triTranslated.p1.z;

		line2.x = triTranslated.p3.x - triTranslated.p1.x;
		line2.y = triTranslated.p3.y - triTranslated.p1.y;
		line2.z = triTranslated.p3.z - triTranslated.p1.z;

		normal = CrossProduct_3D(line1, line2);
		// TODO: implement dot-product, and proper rendering decision
		// based on a proper camera. For now, we are just comparing
		// the z-component of the normal

		if( (normal.x * (triTranslated.p1.x - vCamera.x) + 
			normal.y * (triTranslated.p1.y - vCamera.y) + 
			normal.z * (triTranslated.p1.z - vCamera.z)) < 0.0f)
		{
			// Rasterize triangle -- note, we need 2D triangles
			// TODO: update this call once the camera has been implemented
			uint32_t color;

			// This is also somewhat of a hack
			// TODO: implement proper lighting and camera effects. For now, we're just scaling
			// the brightness of the white based on the calculated z component of the normal vector
			// for each triangle (our light source is 0,0,1)
			color = ( (((uint32_t)((base_color & 0xff0000 >> 4) * -normal.z) << 4) & 0xff0000) |
					  (((uint32_t)((base_color & 0x00ff00 >> 2) * -normal.z) << 2) & 0x00ff00) |
					  (((uint32_t)((base_color & 0x0000ff) * -normal.z)) & 0xff) );
			//DrawTriangle_3D(Buffer, triTranslated.p1, triTranslated.p2, triTranslated.p3, projMatrix, color);			
			FillTriangle_3D(Buffer, triTranslated.p1, triTranslated.p2, triTranslated.p3, projMatrix, color);
		}
	}	
}

internal void 
GameUpdateAndRender(game_memory *Memory, game_offscreen_buffer *Buffer, real32 delta_time)
{
	// We should do some better initialization
	if(!initialized) {Init(Memory, Buffer);}

	// These are just some test drawings to make sure our drawing functionality works!
	FillColor(Buffer, BLACK);

	if(GlobalRotating)
	{
		fTheta += 1.0f * (delta_time / (1000 * 1000));
	}

	DrawCube(Buffer, GREEN);

	// This is called 'per-frame'
}