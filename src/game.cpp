#include "game.h"
#include <windows.h> // TEMPORARY - TO BE REMOVED

#define ROTZPOS 0b0001
#define ROTZNEG 0b0011

#define ROTXNEG 0b1100
#define ROTXPOS 0b1000

// Temporarily using these globals, will find a better system later
global_variable Mesh cube;
global_variable real32 projMatrix[4][4] = {0};
global_variable real32 zTheta = 1.0f;
global_variable real32 xTheta = 1.0f;
global_variable Vec3D vCamera = {0};
global_variable unsigned char Rotation = 0x0000;

global_variable real32 point_rotTheta = 0.0f;
global_variable real32 point_rotSpeed = 1.75f;
global_variable unsigned char rpoint = 1;

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

#if 0
internal void
DrawTriangles(game_offscreen_buffer *Buffer)
{
	// Code that tests out filling individual triangles
	// Here. we've identified an issue where the ordering of the points matters
	// although it really shouldn't!
	Point_2D_Real p1,p2,p3;


	// Not problematic
	p1.x = 150; p1.y = 100;
	p2.x = 200; p2.y = 100;
	p3.x = 120; p3.y = 0;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);
	DrawTriangle_2D(Buffer, p1, p2, p3, GREEN);

	// Problematic (flattop triangle, goes too wide)
	p1.x = 450; p1.y = 100;
	p2.x = 400; p2.y = 100;
	p3.x = 520; p3.y = 0;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);
	DrawTriangle_2D(Buffer, p1, p2, p3, GREEN);

	// Illustrates the issue with a flattop triangle
	p1.x = 700; p1.y = 95;
	p2.x = 800; p2.y = 0;
	p3.x = 900; p3.y = 95;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);
	DrawTriangle_2D(Buffer, p1, p2, p3, GREEN);

	// Illustrates the issue with a flatbottom triangle
	p1.x = 950; p1.y = 0;
	p2.x = 1050; p2.y = 75;
	p3.x = 1150; p3.y = 0;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);
	DrawTriangle_2D(Buffer, p1, p2, p3, GREEN);

	// Problematic (flatbottom triangle, doesn't go wide enough)
	p1.x = 250; p1.y = 10;
	p2.x = 307; p2.y = 70;
	p3.x = 178; p3.y = 150;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);
	DrawTriangle_2D(Buffer, p1, p2, p3, GREEN);

	// Problematic (flatbottom triangle, doesn't go wide enough)
	p1.x = 75; p1.y = 200;
	p2.x = 0; p2.y = 300;
	p3.x = 150; p3.y = 400;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);
	DrawTriangle_2D(Buffer, p1, p2, p3, GREEN);

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
	FillTriangle_2D(Buffer, p1, p2, p3, BLUE);

	p1.x = 200+150*cosf(fTheta); p1.y = 400+150*sinf(fTheta);
	p2.x = 150; p2.y = 305;
	p3.x = 290; p3.y = 350;
	FillTriangle_2D(Buffer, p1, p2, p3, RED);

	p1.x = 500; p1.y = 100;
	p2.x = 551/*+100*cosf(fTheta)*/; p2.y = 250+150*sinf(fTheta);
	p3.x = 501+100*cosf(fTheta); p3.y = 350;
	FillTriangle_2D(Buffer, p1, p2, p3, WHITE);	
}
#endif

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

	// Rotation Z POSITIVE
	matRotZ[0][0] = cosf(zTheta);
	matRotZ[0][1] = sinf(zTheta);
	matRotZ[1][0] = -sinf(zTheta);
	matRotZ[1][1] = cosf(zTheta);
	matRotZ[2][2] = 1;
	matRotZ[3][3] = 1;

	// Rotation X POSITIVE
	matRotX[0][0] = 1;
	matRotX[1][1] = cosf(xTheta * 0.75f);
	matRotX[1][2] = sinf(xTheta * 0.75f);
	matRotX[2][1] = -sinf(xTheta * 0.75f);
	matRotX[2][2] = cosf(xTheta * 0.75f);
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

			FillTriangle_3D(Buffer, triTranslated.p1, triTranslated.p2, triTranslated.p3, projMatrix, color);
			//DrawTriangle_3D(Buffer, triTranslated.p1, triTranslated.p2, triTranslated.p3, projMatrix, BLACK);
		}
	}	
}

internal void
DrawRotatingPoints(game_offscreen_buffer *Buffer, real32 rotTheta)
{
	Point_2D_Int a,b,c,d;
	Point_2D_Int diffColor;
	a.x = 300; a.y = 300;
	b.x = 300; b.y = 400;
	c.x = 400; c.y = 400;
	d.x = 400; d.y = 300;

	switch(rpoint)
	{
		case 1:
		{
			b = RotatePoint_2D(b, rotTheta, a);
			c = RotatePoint_2D(c, rotTheta, a);
			d = RotatePoint_2D(d, rotTheta, a);
			diffColor = a;
		} break;
		case 2: // This case breaks
		{
			a = RotatePoint_2D(a, rotTheta, b);
			c = RotatePoint_2D(c, rotTheta, b);
			d = RotatePoint_2D(d, rotTheta, b);
			diffColor = b;
		} break;
		case 3:
		{
			a = RotatePoint_2D(a, rotTheta, c);
			b = RotatePoint_2D(b, rotTheta, c);
			d = RotatePoint_2D(d, rotTheta, c);
			diffColor = c;
		} break;
		case 4: // As does this one
		{
			a = RotatePoint_2D(a, rotTheta, d);
			b = RotatePoint_2D(b, rotTheta, d);
			c = RotatePoint_2D(c, rotTheta, d);
			diffColor = d;
		} break;
	}

	// Theory behind the issues - is that the order of the points matters when building the rectangle
	// because the rectangle is being drawn as two triangles

	Rect_2D_Int r;
	r.p1 = a; r.p2 = b; r.p3 = c; r.p4 = d;
	FillRect_2D(Buffer, r, WHITE);

	// Indicate which pixel is the current pivot point
	Point_2D_Int start, end;
	start.x = diffColor.x-2; start.y = diffColor.y+2;
	end.x = diffColor.x+2; end.y = diffColor.y-2;
	FillRect_2D(Buffer, start, end, RED);

	//DrawPixel(Buffer, a, WHITE);
	//DrawPixel(Buffer, b, WHITE);
}

internal void 
GameUpdateAndRender(game_memory *Memory, game_offscreen_buffer *Buffer, real32 delta_time, game_keyboard_input *Input)
{
	// This is called 'per-frame'

	// We should do some better initialization
	if(!initialized) {Init(Memory, Buffer);}

	// Here, we should check what kind of input has been provided, and update
	// our state accordingly
	if(Input->keys_pressed[ESC] || Input->keys_pressed[Q]) { exit(0); }

	for(uint32_t i=0; i<Input->num_keys; i++)
	{
		if(Input->keys_pressed[i])
		{
			/* HANDLE INPUT */
			if(i == LEFT){
				Rotation |= ROTZNEG;
			}
			else if(i == RIGHT)
			{
				Rotation &= ~ROTZNEG;
				Rotation |= ROTZPOS;
			}

			// MODIFY X-AXIS ROTATION
			else if(i == DOWN){
				Rotation &= ~ROTXNEG;
				Rotation |= ROTXPOS;
			}
			else if(i == UP)
			{
				Rotation |= ROTXNEG;
			}

			// PAUSE ROTATION
			else if(i == S)
			{
				Rotation = 0x0000;
			}

			// RESET POSITION
			else if(i == R)
			{
				xTheta = 1.0f;
				zTheta = 1.0f;
				point_rotTheta = 0.0f;
			}

			// Switch out to a different model
			//else if(i == ONE)
			//{
			//	DEBUGPlatformReadEntireFile("D:\\dev\\TASKS.txt");
			//}

			else if(i == ONE)
			{
				Rotation = 0x0000;
				point_rotTheta = 0.0f;
				rpoint = 1;
			}
			else if(i == TWO)
			{
				Rotation = 0x0000;
				point_rotTheta = 0.0f;
				rpoint = 2;
			}
			else if(i == THREE)
			{
				Rotation = 0x0000;
				point_rotTheta = 0.0f;
				rpoint = 3;
			}
			else if(i == FOUR)
			{
				Rotation = 0x0000;
				point_rotTheta = 0.0f;
				rpoint = 4;
			}

			/* CLEAR INPUT TO "CONSUME" THE KEYPRESS */
			Input->keys_pressed[i] = false;
		}
		if(Input->keys_released[i]) 
		{ 
			//OutputDebugString("Key release found");
			/* HANDLE INPUT - KEY RELEASE */

			/* CLEAR INPUT TO "CONSUME" THE KEY RELEASE */
			Input->keys_released[i] = false;
		}
	}

	// These are just some test drawings to make sure our drawing functionality works!
	FillColor(Buffer, BLACK);

	// update theta to allow for rotation
	// We tried to be tricky with our definitions of ROTXNEG and ROTZNEG
	// but that could come back to bite us later. 
	// TODO: Improve so we aren't reliant on special rotation encodings
	// that I'll most certain forget in the future
	if(Rotation)
	{
		unsigned char x_check = Rotation & ROTXNEG;
		unsigned char z_check = Rotation & ROTZNEG;
		// Handle X axis rotation
		if(x_check)
		{
			if((x_check) < ROTXNEG) xTheta += 1.0f * (delta_time / (1000 * 1000));
			else xTheta -= 1.0f * (delta_time / (1000 * 1000));
		}

		// Handle Z axis rotation
		if(z_check)
		{
			if((z_check) < ROTZNEG){
				zTheta += 1.0f * (delta_time / (1000 * 1000));
				point_rotTheta += point_rotSpeed * (delta_time / (1000 * 1000));
			}
			else{ 
				zTheta -= 1.0f * (delta_time / (1000 * 1000));
				point_rotTheta -= point_rotSpeed * (delta_time / (1000 * 1000));
			}
		}
	}

	DrawRotatingPoints(Buffer, point_rotTheta);
	//DrawTriangles(Buffer);
	//DrawCube(Buffer, PURPLE);

}