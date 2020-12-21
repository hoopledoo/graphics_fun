#include "drawing.h"

internal void
DrawPixel(game_offscreen_buffer *Buffer, int x, int y, uint32_t color)
{
	if (x < 0 || y < 0 || x >= Buffer->Width || y >= Buffer->Height) return;
	uint8_t *Row = (uint8_t *)(Buffer->Memory) + (y * (Buffer->Pitch));
	uint32_t *Pixel = (uint32_t *)Row + x;
    *Pixel = color;
}

internal void
DrawLine_2D(game_offscreen_buffer *Buffer, Point_2D p1, Point_2D p2, uint32_t color)
{
	int x, y, dx, dy, abs_dx, abs_dy, px, py, end_x, end_y, yi, xi, D;
	
	//Bounds Check
	if(p1.x < 0 || p1.x >= Buffer->Width || p2.x < 0 || p2.x >= Buffer->Width ||
		p1.y < 0 || p1.y >= Buffer->Width || p2.y < 0 || p2.y >= Buffer->Width) 
	{
		return;
	}
	
	dx = p2.x-p1.x;
	dy = p2.y-p1.y;

	if(dx == 0){
		// Handle vertical case

		//Swap the direction if necessary
		if(dy < 0) {y=p2.y; end_y=p1.y;}
		else {y=p1.y; end_y=p2.y;}

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

		uint8_t *Row = (uint8_t *)Buffer->Memory + (p1.y * Buffer->Pitch);
		uint32_t *Pixel = (uint32_t *)Row + x;
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
   	int dx, dy, x, y, start_x, start_y, end_x, end_y;
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

    for(int  y=0; y < Buffer->Height; ++y)
    {
        uint32_t *Pixel = (uint32_t *)Row; // point to the start of the row
        for(int x=0; x < Buffer->Width; ++x)
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
