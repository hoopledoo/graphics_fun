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
		for(; x < p2.x; ++x)
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
DrawTriangle_3D(game_offscreen_buffer *Buffer, Point_3D p1, Point_3D p2, Point_3D p3, uint32_t color)
{
	Triangle_2D translatedTri = {{p1.x,p1.y}, {p2.x, p2.y}, {p3.x, p3.y}};
	DrawTriangle_2D(Buffer, translatedTri.p1, translatedTri.p2, translatedTri.p3, color);
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
