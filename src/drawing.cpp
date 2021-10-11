#include "drawing.h"

// Enforce strictly printing pixels by integer coordinates
internal void
DrawPixel(game_offscreen_buffer *Buffer, int32_t x, int32_t y, uint32_t color)
{
	if (x < 0 || y < 0 || x >= Buffer->Width || y >= Buffer->Height) return;
	uint8_t *Row = (uint8_t *)(Buffer->Memory) + (y * (Buffer->Pitch));
	uint32_t *Pixel = (uint32_t *)Row + x;
    *Pixel = color;
}

internal void 
DrawPixel(game_offscreen_buffer *Buffer, Point_2D_Int p, uint32_t color)
{
	DrawPixel(Buffer, p.x, p.y, color);
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

internal Point_2D_Int
RotatePoint_2D(Point_2D_Int p, real32 angle, Point_2D_Int o)
{
	Point_2D_Real newp;
	real32 s = (real32)sin(angle);
	real32 c = (real32)cos(angle);

	// translate point to the origin
	p.x -= o.x;
	p.y -= o.y;

	// rotate our point
	newp.x = p.x*c - p.y*s;
	newp.y = p.x*s + p.y*c;

	// translate the point back out
	p.x = ROUND_INT(newp.x + o.x);
	p.y = ROUND_INT(newp.y + o.y);

	return p;
}

internal Point_2D_Int
RotatePoint_2D(Point_2D_Real p, real32 angle, Point_2D_Real o)
{
	Point_2D_Real newp;
	Point_2D_Int rotp;
	real32 s = (real32)sin(angle);
	real32 c = (real32)cos(angle);

	// translate point to the origin
	p.x -= o.x;
	p.y -= o.y;

	// rotate our point
	newp.x = p.x*c - p.y*s;
	newp.y = p.x*s + p.y*c;

	// translate the point back out
	rotp.x = ROUND_INT(newp.x + o.x);
	rotp.y = ROUND_INT(newp.y + o.x);

	return rotp;
}

// Convert to Integer-based Points before drawing the line
internal void
DrawLine_2D(game_offscreen_buffer *Buffer, Point_2D_Real p1_real, Point_2D_Real p2_real, uint32_t color)
{
	Point_2D_Int p1,p2;
	p1.x = ROUND_INT(p1_real.x); p1.y = ROUND_INT(p1_real.y);
	p2.x = ROUND_INT(p2_real.x); p2.y = ROUND_INT(p2_real.y);
	DrawLine_2D(Buffer, p1, p2, color);
}

// Drawing with Integer-based Points
internal void
DrawLine_2D(game_offscreen_buffer *Buffer, Point_2D_Int p1, Point_2D_Int p2, uint32_t color)
{
	int32_t x, y, dx, dy, abs_dx, abs_dy, px, py, end_x, end_y, yi, xi, D;
	
	dx = p2.x-p1.x;
	dy = p2.y-p1.y;

	if(dx == 0){
		// Handle vertical case

		//Swap the direction if necessary
		if(dy < 0) {y=p2.y; end_y=p1.y;}
		else {y=p1.y; end_y=p2.y;}

		// Clip the lines at the boundaries of the window height
		if(y < 0) {y = 0;}
		else if (y >= Buffer->Height) {y = (Buffer->Height - 1);}

		if(end_y < 0) {end_y = 0;}
		else if (end_y >= Buffer->Height) {end_y = (Buffer->Height - 1);}

		uint8_t *Row = (uint8_t *)Buffer->Memory + (y * Buffer->Pitch);
		Row = (uint8_t *)((uint32_t *)Row + p1.x);
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
		if(x < 0) {x = 0;}
		else if (x >= Buffer->Width) {x = (Buffer->Width - 1);}

		if(end_x < 0) {end_x = 0;}
		else if (end_x >= Buffer->Width) {end_x = (Buffer->Width - 1);}

		uint8_t *Row = (uint8_t *)Buffer->Memory + (p1.y * Buffer->Pitch);
		uint32_t *Pixel = (uint32_t *)Row + x;
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
DrawTriangle_2D(game_offscreen_buffer *Buffer, Point_2D_Real p1_real, Point_2D_Real p2_real, Point_2D_Real p3_real, uint32_t color)
{
	//Convert to Integer-based Points first
	Point_2D_Int p1, p2, p3;
	p1.x = ROUND_INT(p1_real.x); p1.y = ROUND_INT(p1_real.y);
	p2.x = ROUND_INT(p2_real.x); p2.y = ROUND_INT(p2_real.y);
	p3.x = ROUND_INT(p3_real.x); p3.y = ROUND_INT(p3_real.y);

	DrawLine_2D(Buffer, p1, p2, color);
	DrawLine_2D(Buffer, p2, p3, color);
	DrawLine_2D(Buffer, p3, p1, color);
}

internal void
DrawTriangle_2D(game_offscreen_buffer *Buffer, Point_2D_Int p1, Point_2D_Int p2, Point_2D_Int p3, uint32_t color)
{
	DrawLine_2D(Buffer, p1, p2, color);
	DrawLine_2D(Buffer, p2, p3, color);
	DrawLine_2D(Buffer, p3, p1, color);
}


internal void
FillTriangle_2D(game_offscreen_buffer *Buffer, Point_2D_Real p1_real, Point_2D_Real p2_real, Point_2D_Real p3_real, uint32_t color)
{
	//Convert to Integer-based Points first (this is effectively to round it and eliminate the gap line we were seeing)
	Point_2D_Int p1, p2, p3;
	p1.x = ROUND_INT(p1_real.x); p1.y = ROUND_INT(p1_real.y);
	p2.x = ROUND_INT(p2_real.x); p2.y = ROUND_INT(p2_real.y);
	p3.x = ROUND_INT(p3_real.x); p3.y = ROUND_INT(p3_real.y);

	FillTriangle_2D(Buffer, p1, p2, p3, color);
}

// If we want to save some cycles, we can use the naive method to fill triangles
internal void
FillFlatTopNaive(game_offscreen_buffer *Buffer, 
			Point_2D_Real flattop_start, Point_2D_Real flattop_end1, Point_2D_Real flattop_end2,
			uint32_t color)
{
	real32 invslope1 = (flattop_end1.x - flattop_start.x) / (flattop_end1.y - flattop_start.y);
	real32 invslope2 = (flattop_end2.x - flattop_start.x) / (flattop_end2.y - flattop_start.y);

	Point_2D_Real curPoint1, curPoint2;
	curPoint1.x = flattop_start.x;
	curPoint2.x = flattop_start.x;

	for(real32 y = flattop_start.y; y < flattop_end1.y; y++)
	{
		curPoint1.y = y;
		curPoint2.y = y;
		DrawLine_2D(Buffer, curPoint1, curPoint2, color);
		curPoint1.x += invslope1;
		curPoint2.x += invslope2;
	}
}

// We need to convert the points from Real to Int first
internal void
FillFlatTopBresenham(game_offscreen_buffer *Buffer, 
			Point_2D_Real start, Point_2D_Real end1, Point_2D_Real end2,
			uint32_t color)
{
	Point_2D_Int p1, p2, p3;
	p1.x = ROUND_INT(start.x); p1.y = ROUND_INT(start.y);
	p2.x = ROUND_INT(end1.x); p2.y = ROUND_INT(end1.y);
	p3.x = ROUND_INT(end2.x); p3.y = ROUND_INT(end2.y);

	FillFlatTopBresenham(Buffer, p1, p2, p3, color);
}

// We know that the y-value will be increasing from start -> end1 and end2
// We only need to handle the top 2 quadrants
internal void
FillFlatTopBresenham(game_offscreen_buffer *Buffer, 
			Point_2D_Int start, Point_2D_Int end1, Point_2D_Int end2,
			uint32_t color)
{
	int32_t line1_dx, line2_dx, line1_abs_dx, line2_abs_dx, dy;
	int32_t line1_px, line1_py, line2_px, line2_py, line1_xi, line2_xi;
	int32_t line1x, line2x, y;
	Point_2D_Int p1, p2;

	// initialize line-walkers
	y = start.y; line1x = start.x; line2x = start.x;

	// calculate the deltas
	dy = end1.y - start.y;
	line1_dx = end1.x - start.x; line1_abs_dx = ABS(line1_dx);
	line2_dx = end2.x - start.x; line2_abs_dx = ABS(line2_dx);

	// Calculate the bresenham error values
	line1_py = 2*line1_abs_dx - dy;
	line2_py = 2*line2_abs_dx - dy;
	line1_px = 2*dy - line1_abs_dx;
	line2_px = 2*dy - line2_abs_dx;

	// Update the x incrementer based on the line's x delta
	if(line1_dx < 0) line1_xi = -1;
	else line1_xi = 1;
	if(line2_dx < 0) line2_xi = -1;
	else line2_xi = 1;
	DrawPixel(Buffer, start.x, start.y, color);

	while(y < end1.y){
		//--------------- walk line 1 -----------------------

		// ensure it's not a vertical line
		if(line1_dx !=0){

			// handle steep line2
			if(line1_abs_dx <= dy){
				if(line1_py < 0){
					line1_py = line1_py + 2*line1_abs_dx;
				}
				else{
					line1x = line1x + line1_xi;
					line1_py = line1_py + 2*(line1_abs_dx - dy);
				}
			}

			// handle shallow line1
			else{
				line1x = line1x + line1_xi;

				while(line1_px <= 0){
					line1x = line1x + line1_xi;
					line1_px = line1_px + 2*dy;
				}

				line1_px = line1_px + 2*(dy - line1_abs_dx);
			}
		}

		//--------------- walk line 2 -----------------------

		// ensure it's not a vertical line
		if(line2_dx !=0){

			// handle steep line2
			if(line2_abs_dx <= dy){
				if(line2_py < 0){
					line2_py = line2_py + 2*line2_abs_dx;
				}
				else{
					line2x = line2x + line2_xi;
					line2_py = line2_py + 2*(line2_abs_dx - dy);
				}
			}

			// handle shallow line2
			else{
				line2x = line2x + line2_xi;

				while(line2_px <= 0){
					line2x = line2x + line2_xi;
					line2_px = line2_px + 2*dy;
				}
				
				line2_px = line2_px + 2*(dy - line2_abs_dx);
			}
		}

		// Step down
		y++;
		p1.x = line1x; p1.y = y;
		p2.x = line2x; p2.y = y;
		DrawLine_2D(Buffer, p1, p2, color);
	}
}

// If we want to save some cycles, we can use the naive method to fill triangles
internal void
FillFlatBottomNaive(game_offscreen_buffer *Buffer,
				Point_2D_Real flatbottom_start, Point_2D_Real flatbottom_end1, Point_2D_Real flatbottom_end2,
				uint32_t color)
{
	real32 invslope1 = (flatbottom_start.x - flatbottom_end1.x) / (flatbottom_start.y - flatbottom_end1.y);
	real32 invslope2 = (flatbottom_start.x - flatbottom_end2.x) / (flatbottom_start.y - flatbottom_end2.y);

	Point_2D_Real curPoint1, curPoint2;
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
}

// Convert points to integers, because Bresenham works on integers only
internal void
FillFlatBottomBresenham(game_offscreen_buffer *Buffer,
				Point_2D_Real start, Point_2D_Real end1, Point_2D_Real end2,
				uint32_t color)
{
	Point_2D_Int p1, p2, p3;
	p1.x = ROUND_INT(start.x); p1.y = ROUND_INT(start.y);
	p2.x = ROUND_INT(end1.x); p2.y = ROUND_INT(end1.y);
	p3.x = ROUND_INT(end2.x); p3.y = ROUND_INT(end2.y);

	FillFlatBottomBresenham(Buffer, p1, p2, p3, color);
}

// We know the y value will be decreasing from start -> end1 and end2
// We only need to handle the bottom two quadrants
internal void
FillFlatBottomBresenham(game_offscreen_buffer *Buffer, 
			Point_2D_Int start, Point_2D_Int end1, Point_2D_Int end2,
			uint32_t color)
{
	int32_t line1_dx, line2_dx, line1_abs_dx, line2_abs_dx, dy;
	int32_t line1_px, line1_py, line2_px, line2_py, line1_xi, line2_xi;
	int32_t line1x, line2x, y;
	Point_2D_Int p1, p2;

	// initialize line-walkers
	y = start.y; line1x = start.x; line2x = start.x;

	// calculate the deltas
	dy = start.y - end1.y;
	line1_dx = end1.x - start.x; line1_abs_dx = ABS(line1_dx);
	line2_dx = end2.x - start.x; line2_abs_dx = ABS(line2_dx);

	// Calculate the bresenham error values
	line1_py = 2*line1_abs_dx - dy;
	line2_py = 2*line2_abs_dx - dy;
	line1_px = 2*dy - line1_abs_dx;
	line2_px = 2*dy - line2_abs_dx;

	// Update the x incrementer based on the line's x delta
	if(line1_dx < 0) line1_xi = -1;
	else line1_xi = 1;
	if(line2_dx < 0) line2_xi = -1;
	else line2_xi = 1;
	DrawPixel(Buffer, start.x, start.y, color);

	while(y > end1.y){
		//--------------- walk line 1 -----------------------

		// ensure it's not a vertical line
		if(line1_dx !=0){

			// handle steep line2
			if(line1_abs_dx <= dy){
				if(line1_py < 0){
					line1_py = line1_py + 2*line1_abs_dx;
				}
				else{
					line1x = line1x + line1_xi;
					line1_py = line1_py + 2*(line1_abs_dx - dy);
				}
			}

			// handle shallow line1
			else{
				line1x = line1x + line1_xi;

				while(line1_px <= 0){
					line1x = line1x + line1_xi;
					line1_px = line1_px + 2*dy;
				}

				line1_px = line1_px + 2*(dy - line1_abs_dx);
			}
		}

		//--------------- walk line 2 -----------------------

		// ensure it's not a vertical line
		if(line2_dx !=0){

			// handle steep line2
			if(line2_abs_dx <= dy){
				if(line2_py < 0){
					line2_py = line2_py + 2*line2_abs_dx;
				}
				else{
					line2x = line2x + line2_xi;
					line2_py = line2_py + 2*(line2_abs_dx - dy);
				}
			}

			// handle shallow line2
			else{
				line2x = line2x + line2_xi;

				while(line2_px <= 0){
					line2x = line2x + line2_xi;
					line2_px = line2_px + 2*dy;
				}
				
				line2_px = line2_px + 2*(dy - line2_abs_dx);
			}
		}

		// Step down
		y--;
		p1.x = line1x; p1.y = y;
		p2.x = line2x; p2.y = y;
		DrawLine_2D(Buffer, p1, p2, color);
	}
}

// We'll use Bresenham's algorithm again. Essentially, we'll step through one line until there's been a change
// in y, then we'll step through the other until there's been a change in y.
// Once there has been a change in both y's, we'll draw a horizontal line between the two.
// We'll continue until we reach the end of the lines
internal void
FillTriangle_2D(game_offscreen_buffer *Buffer, Point_2D_Int p1_int, Point_2D_Int p2_int, Point_2D_Int p3_int, uint32_t color)
{
	// We need to work in real coordinates, but we want to make sure everything was rounded first
	// here, we'll convert back to floating point
	Point_2D_Real p1, p2, p3;
	p1.x = (real32)p1_int.x; p1.y = (real32)p1_int.y;
	p2.x = (real32)p2_int.x; p2.y = (real32)p2_int.y;
	p3.x = (real32)p3_int.x; p3.y = (real32)p3_int.y;

	// TODO: migrate to a new method using Bresenham's algorithm (the current naive 
	// method contains glitching lines because it imperfectly steps through)

	// We first need to find if this is a flat-top, flat-bottom, or if we need to split it
	real32 dy_p1p2, dy_p1p3, dy_p2p3, dx_p1p2, dx_p1p3, dx_p2p3;
	Point_2D_Real flattop_start, flattop_end1, flattop_end2, flatbottom_start, flatbottom_end1, flatbottom_end2;
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
		Point_2D_Real top_point, middle_point, bottom_point, p4;

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
		FillFlatTopBresenham(Buffer, flattop_start, flattop_end1, flattop_end2, color);
		DrawTriangle_2D(Buffer, flattop_start, flattop_end1, flattop_end2, color);
	}
	if(flatbottom)
	{
		FillFlatBottomBresenham(Buffer, flatbottom_start, flatbottom_end1, flatbottom_end2, color);
		DrawTriangle_2D(Buffer, flatbottom_start, flatbottom_end1, flatbottom_end2, color);
	}

}

// TODO: improve this so it takes a camera and performs the translations we want
internal void
DrawTriangle_3D(game_offscreen_buffer *Buffer, Point_3D p1, Point_3D p2, Point_3D p3, real32 projMatrix[4][4], uint32_t color)
{
	Triangle_3D triProjected = {0};
	Triangle_2D_Real triProjected_2D = {0};

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
	Triangle_2D_Real triProjected_2D = {0};

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
FillRect_2D(game_offscreen_buffer *Buffer, Rect_2D_Int r, uint32_t color)
{
	FillTriangle_2D(Buffer, r.p1, r.p2, r.p3, color);
	FillTriangle_2D(Buffer, r.p1, r.p3, r.p4, color);
}

internal void
FillRect_2D(game_offscreen_buffer *Buffer, Rect_2D_Real r, uint32_t color)
{
	FillTriangle_2D(Buffer, r.p1, r.p2, r.p3, color);
	FillTriangle_2D(Buffer, r.p1, r.p3, r.p4, color);
}

internal void
FillRect_2D(game_offscreen_buffer *Buffer, Point_2D_Real p1_real, Point_2D_Real p2_real, uint32_t color)
{
	Point_2D_Int p1,p2;
	p1.x = ROUND_INT(p1_real.x); p1.y = ROUND_INT(p1_real.y);
	p2.x = ROUND_INT(p2_real.x); p2.y = ROUND_INT(p2_real.y);
	FillRect_2D(Buffer, p1, p2, color);	
}

internal void
FillRect_2D(game_offscreen_buffer *Buffer, Point_2D_Int p1, Point_2D_Int p2, uint32_t color)
{
   	int32_t dx, dy, x, y, start_x, start_y, end_x, end_y;
   	dx = p2.x-p1.x;
	dy = p2.y-p1.y;

	//Swap the direction if necessary
	if(dx < 0) {start_x = p2.x; end_x = p1.x;}
	else {start_x=p1.x; end_x=p2.x;}

	if(dy < 0) {start_y=p2.y; end_y=p1.y;}
	else {start_y=p1.y; end_y=p2.y;}

    uint8_t *Row = (uint8_t *)Buffer->Memory + (start_y * Buffer->Pitch); // get a byte pointer to memory

    for(y=start_y; y<end_y; ++y)
    {
        uint32_t *Pixel = (uint32_t *)Row + start_x; // point to the start of the row
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
