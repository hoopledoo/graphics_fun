#include "drawing.h"

internal void
DrawPixel(game_offscreen_buffer *Buffer, real32 x, real32 y, uint32_t color)
{
	if (x < 0 || y < 0 || x >= Buffer->Width || y >= Buffer->Height) return;
	uint8_t *Row = (uint8_t *)(Buffer->Memory) + ((int)y * (Buffer->Pitch));
	uint32_t *Pixel = (uint32_t *)Row + (int)x;
    *Pixel = color;
}

internal void
MatrixVecMult(Point_3D *out_point, Point_3D *in_point, real32 m[4][4])
{
	out_point->x = in_point->x * m[0][0] + in_point->y * m[1][0] + in_point->z * m[2][0] + m[3][0];
	out_point->y = in_point->x * m[0][1] + in_point->y * m[1][1] + in_point->z * m[2][1] + m[3][1];
	out_point->z = in_point->x * m[0][2] + in_point->y * m[1][2] + in_point->z * m[2][2] + m[3][2];
	real32 w = in_point->x * m[0][3] + in_point->y * m[1][3] + in_point->z * m[2][3] + m[3][3];

	if (w != 0.0f)
	{
		out_point->x /= w; out_point->y /= w; out_point->z /= w;
	}
}

internal Vec3D
CrossProduct_3D(Vec3D v1, Vec3D v2)
{
	Vec3D normal;
	normal.x = v1.y * v2.z - v1.z * v2.y;
	normal.y = v1.z * v2.x - v1.x * v2.z;
	normal.z = v1.x * v2.y - v1.y * v2.x;

	// Normalize our result
	float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
	normal.x /= l; normal.y /= l; normal.z /= l;
	return normal;
}

internal void
DrawLine_2D(game_offscreen_buffer *Buffer, Point_2D p1, Point_2D p2, uint32_t color)
{
	real32 x, y, dx, dy, abs_dx, abs_dy, px, py, end_x, end_y, yi, xi, D;
	
	dx = p2.x-p1.x;
	dy = p2.y-p1.y;

	if(dx == 0){
		// Handle vertical case

		//Swap the direction if necessary
		if(dy < 0) {y=p2.y; end_y=p1.y;}
		else {y=p1.y; end_y=p2.y;}

		// Clip the lines at the boundaries of the window height
		if(y < 0) {y = 0.0f;}
		else if (y >= Buffer->Height) {y = (real32)(Buffer->Height - 1);}

		if(end_y < 0) {end_y = 0.0f;}
		else if (end_y >= Buffer->Height) {end_y = (real32)(Buffer->Height - 1);}

		uint8_t *Row = (uint8_t *)Buffer->Memory + ((int)y * Buffer->Pitch);
		Row = (uint8_t *)((uint32_t *)Row + (int)p1.x);
		for(; y < end_y; ++y)
		{
			*(uint32_t *)Row = color;
			Row += Buffer->Pitch;
		}
	}
	else if(dy == 0){
		// Handle horizontal case

		// Swap the direction if necessary
		if(dx < 0) {x = p2.x; end_x = p1.x;}
		else {x=p1.x; end_x=p2.x;}

		// Clip the lines at the boundaries of the window width
		if(x < 0) {x = 0.0f;}
		else if (x >= Buffer->Width) {x = (real32)(Buffer->Width - 1);}

		if(end_x < 0) {end_x = 0.0f;}
		else if (end_x >= Buffer->Width) {end_x = (real32)(Buffer->Width - 1);}

		uint8_t *Row = (uint8_t *)Buffer->Memory + ((int)p1.y * Buffer->Pitch);
		uint32_t *Pixel = (uint32_t *)Row + (int)x;
		for(; x < end_x; ++x)
		{
			*Pixel = color;
			Pixel++;
		}
	}
	else{
		// Here we handle lines that are neither vertical nor horizontal
		// and we'll use Bresenham algorithm
		abs_dx = ABS(dx);
		abs_dy = ABS(dy);
		px = 2*abs_dy - abs_dx;
		py = 2*abs_dx - abs_dy;

		// Handle shallow lines 
		if(abs_dy <= abs_dx)
		{
			if(dx>0) {x = p1.x; y = p1.y; end_x = p2.x;}
			else {x = p2.x; y = p2.y; end_x = p1.x;}

			if((dx<0 && dy<0) || (dx>0 && dy>0)) yi = 1;
			else yi = -1;

			for(; x<end_x; x++)
			{
				DrawPixel(Buffer, x, y, color);
				if(px < 0)
				{
					px = px + 2*abs_dy;
				}
				else
				{
					y = y + yi;
					px = px + 2*(abs_dy - abs_dx);
				} 
			}
		}
		// Handle steep lines
		else
		{
			if(dy>0) {x = p1.x; y = p1.y; end_y = p2.y;}
			else {x = p2.x; y = p2.y; end_y = p1.y;}

			if((dx<0 && dy<0) || (dx>0 && dy>0)) xi = 1;
			else xi = -1;

			for(; y<end_y; y++)
			{
				DrawPixel(Buffer, x, y, color);
				if(py < 0)
				{
					py = py + 2*abs_dx;
				}
				else
				{
					x = x + xi;
					py = py + 2*(abs_dx - abs_dy);
				} 
			}
		}
	}
}

internal void
DrawTriangle_2D(game_offscreen_buffer *Buffer, Point_2D p1, Point_2D p2, Point_2D p3, uint32_t color)
{
	DrawLine_2D(Buffer, p1, p2, color);
	DrawLine_2D(Buffer, p2, p3, color);
	DrawLine_2D(Buffer, p3, p1, color);
}

internal void
AdvanceAlongLine(Point_2D start, Point_2D end)
{
	
}

// We'll use Bresenham's algorithm again. Essentially, we'll step through one line until there's been a change
// in y, then we'll step through the other until there's been a change in y.
// Once there has been a change in both y's, we'll draw a horizontal line between the two.
// We'll continue until we reach the end of the lines
internal void
FillTriangle_2D(game_offscreen_buffer *Buffer, Point_2D p1, Point_2D p2, Point_2D p3, uint32_t color)
{
	// TODO: determine why the naive method includes horizontal line gaps when drawing both 
	// parts of the triangle, which occurs consistently but only with certain triangles

	// TODO: migrate to a new method using Bresenham's algorithm (the current naive 
	// method contains glitching lines because it imperfectly steps through)

	// We first need to find if this is a flat-top, flat-bottom, or if we need to split it
	real32 dy_p1p2, dy_p1p3, dy_p2p3, dx_p1p2, dx_p1p3, dx_p2p3;
	Point_2D flattop_start, flattop_end1, flattop_end2, flatbottom_start, flatbottom_end1, flatbottom_end2;
	bool32 flattop = false;
	bool32 flatbottom = false;

	dy_p1p2 = p1.y - p2.y;
	dy_p1p3 = p1.y - p3.y;
	dy_p2p3 = p2.y - p3.y;
	dx_p1p2 = p1.x - p2.x;
	dx_p1p3 = p1.x - p3.x;
	dx_p2p3 = p2.x - p3.x;

	// Check to make sure the triangle isn't just a horizontal line
	if(dy_p1p2 == 0 && dy_p2p3 == 0 && dy_p1p3 == 0) { DrawTriangle_2D(Buffer, p1, p2, p3, color); return;}

	// Check to make sure the triangle isn't just a vertical line
	if(dx_p1p2 == 0 && dx_p2p3 == 0 && dx_p1p3 == 0) { DrawTriangle_2D(Buffer, p1, p2, p3, color); return;}	

	// We have a flat-side for our triangle, now determine if a flat-top or flat-bottom
	if(dy_p1p2 == 0) 
	{ 
		if(dy_p2p3 > 0) { flattop = true; flattop_start = p3; flattop_end1 = p1; flattop_end2 = p2; }
		else { flatbottom = true; flatbottom_start = p3; flatbottom_end1 = p1; flatbottom_end2 = p2; } 
	}
	else if(dy_p2p3 == 0) 
	{ 
		if(dy_p1p3 < 0) { flattop = true; flattop_start = p1; flattop_end1 = p2; flattop_end2 = p3; }
		else { flatbottom = true; flatbottom_start = p1; flatbottom_end1 = p2; flatbottom_end2 = p3; } 	
	}
	else if(dy_p1p3 == 0) 
	{ 
		if(dy_p1p2 > 0) { flattop = true; flattop_start = p2; flattop_end1 = p1; flattop_end2 = p3; }
		else { flatbottom = true; flatbottom_start = p2; flatbottom_end1 = p1; flatbottom_end2 = p3; } 
	}
	
	// This doesn't have a flat-side, so we need to find a flat-side
	else 
	{
		// First, Identify the top-most, middle, and bottom-most points
		Point_2D top_point, middle_point, bottom_point, p4;

		// TODO: improve how we're sorting these points
		if( p1.y > p2.y && p2.y > p3.y ) { top_point = p1; middle_point = p2; bottom_point = p3; }
		else if( p1.y > p3.y && p3.y > p2.y) { top_point = p1; middle_point = p3; bottom_point = p2; }
		else if( p2.y > p3.y && p3.y > p1.y) { top_point = p2; middle_point = p3; bottom_point = p1; }
		else if( p2.y > p1.y && p1.y > p3.y) { top_point = p2; middle_point = p1; bottom_point = p3; }
		else if( p3.y > p1.y && p1.y > p2.y) { top_point = p3; middle_point = p1; bottom_point = p2; }
		else { top_point = p3; middle_point = p2; bottom_point = p1; }

		// Using the middle-point, split the triangle into two triangles
		p4.y = middle_point.y;
		p4.x = top_point.x + ((middle_point.y - top_point.y)/(bottom_point.y - top_point.y)) * (bottom_point.x - top_point.x);	

		flatbottom_start = top_point;
		flatbottom_end1 = middle_point;
		flatbottom_end2 = p4;
		flatbottom = true;

		flattop_start = bottom_point;
		flattop_end1 = middle_point;
		flattop_end2 = p4;
		flattop = true;
	}

	// Here we need to actually draw the flattop & flatbottom triangles
	if(flattop)
	{

		real32 invslope1 = (flattop_end1.x - flattop_start.x) / (flattop_end1.y - flattop_start.y);
		real32 invslope2 = (flattop_end2.x - flattop_start.x) / (flattop_end2.y - flattop_start.y);

		Point_2D curPoint1, curPoint2;
		curPoint1.x = flattop_start.x;
		curPoint2.x = flattop_start.x;

		// Round our ending point up
		if((int)flattop_end1.y != (int)(flattop_end1.y+0.5f))
		{
			//flattop_end1.y += 0.5f;
		}

		for(real32 y = flattop_start.y; y < flattop_end1.y; y++)
		{
			curPoint1.y = y;
			curPoint2.y = y;
			DrawLine_2D(Buffer, curPoint1, curPoint2, color);
			curPoint1.x += invslope1;
			curPoint2.x += invslope2;
		}
	}
	if(flatbottom)
	{
		//#if 0
		real32 invslope1 = (flatbottom_start.x - flatbottom_end1.x) / (flatbottom_start.y - flatbottom_end1.y);
		real32 invslope2 = (flatbottom_start.x - flatbottom_end2.x) / (flatbottom_start.y - flatbottom_end2.y);

		Point_2D curPoint1, curPoint2;
		curPoint1.x = flatbottom_start.x;
		curPoint2.x = flatbottom_start.x;

		// To prevent a split in the triangle, we want to ensure that we go all the way
		// to the end of the flattop triangle
		for(real32 y = flatbottom_start.y; y >= flatbottom_end1.y; y--)
		{
			curPoint1.y = y;
			curPoint2.y = y;
			DrawLine_2D(Buffer, curPoint1, curPoint2, color);
			curPoint1.x -= invslope1;
			curPoint2.x -= invslope2;
		}
		//#endif
	}

}

// TODO: improve this so it takes a camera and performs the translations we want
internal void
DrawTriangle_3D(game_offscreen_buffer *Buffer, Point_3D p1, Point_3D p2, Point_3D p3, real32 projMatrix[4][4], uint32_t color)
{
	Triangle_3D triProjected = {0};
	Triangle_2D triProjected_2D = {0};

	// Project triangles from 3D --> 2D
	MatrixVecMult(&triProjected.p1, &p1, projMatrix);
	MatrixVecMult(&triProjected.p2, &p2, projMatrix);
	MatrixVecMult(&triProjected.p3, &p3, projMatrix);

	// Scale into view - and eliminate the z component
	// TODO: this is a placeholder until a proper camera is implemented
	triProjected_2D.p1.x = triProjected.p1.x + 1.f; triProjected_2D.p1.y = triProjected.p1.y + 1.f;
	triProjected_2D.p2.x = triProjected.p2.x + 1.f; triProjected_2D.p2.y = triProjected.p2.y + 1.f;
	triProjected_2D.p3.x = triProjected.p3.x + 1.f; triProjected_2D.p3.y = triProjected.p3.y + 1.f;
	triProjected_2D.p1.x *= (0.5f * (real32)Buffer->Width);
	triProjected_2D.p1.y *= (0.5f * (real32)Buffer->Height);
	triProjected_2D.p2.x *= (0.5f * (real32)Buffer->Width);
	triProjected_2D.p2.y *= (0.5f * (real32)Buffer->Height);
	triProjected_2D.p3.x *= (0.5f * (real32)Buffer->Width);
	triProjected_2D.p3.y *= (0.5f * (real32)Buffer->Height);

	DrawTriangle_2D(Buffer, triProjected_2D.p1, triProjected_2D.p2, triProjected_2D.p3, color);
}

internal void
FillTriangle_3D(game_offscreen_buffer *Buffer, Point_3D p1, Point_3D p2, Point_3D p3, real32 projMatrix[4][4], uint32_t color)
{
	Triangle_3D triProjected = {0};
	Triangle_2D triProjected_2D = {0};

	// Project triangles from 3D --> 2D
	MatrixVecMult(&triProjected.p1, &p1, projMatrix);
	MatrixVecMult(&triProjected.p2, &p2, projMatrix);
	MatrixVecMult(&triProjected.p3, &p3, projMatrix);

	// Scale into view - and eliminate the z component
	// TODO: this is a placeholder until a proper camera is implemented
	triProjected_2D.p1.x = triProjected.p1.x + 1.f; triProjected_2D.p1.y = triProjected.p1.y + 1.f;
	triProjected_2D.p2.x = triProjected.p2.x + 1.f; triProjected_2D.p2.y = triProjected.p2.y + 1.f;
	triProjected_2D.p3.x = triProjected.p3.x + 1.f; triProjected_2D.p3.y = triProjected.p3.y + 1.f;
	triProjected_2D.p1.x *= (0.5f * (real32)Buffer->Width);
	triProjected_2D.p1.y *= (0.5f * (real32)Buffer->Height);
	triProjected_2D.p2.x *= (0.5f * (real32)Buffer->Width);
	triProjected_2D.p2.y *= (0.5f * (real32)Buffer->Height);
	triProjected_2D.p3.x *= (0.5f * (real32)Buffer->Width);
	triProjected_2D.p3.y *= (0.5f * (real32)Buffer->Height);

	FillTriangle_2D(Buffer, triProjected_2D.p1, triProjected_2D.p2, triProjected_2D.p3, color);
}

internal void
FillRect_2D(game_offscreen_buffer *Buffer, Point_2D p1, Point_2D p2, uint32_t color)
{
   	real32 dx, dy, x, y, start_x, start_y, end_x, end_y;
   	dx = p2.x-p1.x;
	dy = p2.y-p1.y;

	//Swap the direction if necessary
	if(dx < 0) {start_x = p2.x; end_x = p1.x;}
	else {start_x=p1.x; end_x=p2.x;}

	if(dy < 0) {start_y=p2.y; end_y=p1.y;}
	else {start_y=p1.y; end_y=p2.y;}

    uint8_t *Row = (uint8_t *)Buffer->Memory + ((int)start_y * Buffer->Pitch); // get a byte pointer to memory

    for(y=start_y; y<end_y; ++y)
    {
        uint32_t *Pixel = (uint32_t *)Row + (int)start_x; // point to the start of the row
        for(x=start_x; x<end_x; ++x)
        {
            *Pixel++ = color;
        } // end for loop through columns
        
        Row += Buffer->Pitch;
    } // end for loop through rows
}

internal void
FillColor(game_offscreen_buffer *Buffer, uint32_t color)
{
    uint8_t *Row = (uint8_t *)Buffer->Memory; // get a byte pointer to memory

    for(real32  y=0; y < Buffer->Height; ++y)
    {
        uint32_t *Pixel = (uint32_t *)Row; // point to the start of the row
        for(real32 x=0; x < Buffer->Width; ++x)
        {
            *Pixel++ = color;
            /*
                Little Endian, so...
                memory:         BB GG RR xx
                register:       xx RR GG BB
            */
            
            //uint8_t Blue= (uint8_t)(x + BlueOffset);
            //uint8_t Green= (uint8_t)(y + GreenOffset);
            
            //*Pixel++ = ((Green << 8) | Blue);
        } // end for loop through columns
        
        Row += Buffer->Pitch;
        
    } // end for loop through rows
}

// TODO: add a draw mesh function that utilizes both the scale and offset
// to draw a mesh
