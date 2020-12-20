#include "game.h"

internal void
DrawPixel(game_offscreen_buffer *Buffer, int x, int y, uint32_t color)
{
	if (x < 0 || y < 0 || x >= Buffer->Width || y >= Buffer->Height) return;
	uint8_t *Row = (uint8_t *)(Buffer->Memory) + (y * (Buffer->Pitch));
	uint32_t *Pixel = (uint32_t *)Row + x;
    *Pixel = color;
}

/* Note: we should improve this by anti-aliasing. */
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
		uint8_t *Row = (uint8_t *)Buffer->Memory + (p1.y * Buffer->Pitch) + p1.x;
		for(y=p1.y; y < p2.y; ++y)
		{
			*(uint32_t *)Row = color;
			Row += Buffer->Pitch;
		}
	}
	else if(dy == 0){
		// Handle horizontal case
		uint8_t *Pixel = (uint8_t *)Buffer->Memory + (p1.y * Buffer->Pitch) + p1.x;
		for(x=p1.x; x < p2.x; ++x)
		{
			*(uint32_t *)Pixel = color;
			Pixel++;
		}
	}
	else{
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

// Performs the projection of the 3D line into 2D space, and then draws the line as a 2D line
internal void
DrawLine_3D(game_offscreen_buffer *Buffer, Point_3D p1, Point_3D p2, uint32_t color)
{
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
	/*
	DrawLine_3D();
	DrawLine_3D();
	DrawLine_3D();
	*/
}

internal void
FillRect_2D(game_offscreen_buffer *Buffer, Point_2D p1, Point_2D p2, uint32_t color)
{
    uint8_t *Row = (uint8_t *)Buffer->Memory + (p1.y * Buffer->Pitch); // get a byte pointer to memory

    for(int  y=p1.y; y < p2.y; ++y)
    {
        uint32_t *Pixel = (uint32_t *)Row + p1.x; // point to the start of the row
        for(int x=p1.x; x < p2.x; ++x)
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