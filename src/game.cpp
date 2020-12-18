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
DrawLine(game_offscreen_buffer *Buffer, int x1, int y1, int x2, int y2, uint32_t color)
{
	int x, y, dx, dy, abs_dx, abs_dy, px, py, end_x, end_y, yi, xi, D;
	
	//Bounds Check
	if(x1 < 0 || x1 >= Buffer->Width || x2 < 0 || x2 >= Buffer->Width ||
		y1 < 0 || y1 >= Buffer->Width || y2 < 0 || y2 >= Buffer->Width) 
	{
		return;
	}
	
	dx = x2-x1;
	dy = y2-y1;

	if(dx == 0){
		// Handle vertical case
		uint8_t *Row = (uint8_t *)Buffer->Memory + (y1 * Buffer->Pitch) + x1;
		for(y=y1; y < y2; ++y)
		{
			*(uint32_t *)Row = color;
			Row += Buffer->Pitch;
		}
	}
	else if(dy == 0){
		// Handle horizontal case
		uint8_t *Pixel = (uint8_t *)Buffer->Memory + (y1 * Buffer->Pitch) + x1;
		for(x=x1; x < x2; ++x)
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
			if(dx>0) {x = x1; y = y1; end_x = x2;}
			else {x = x2; y = y2; end_x = x1;}

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
			if(dy>0) {x = x1; y = y1; end_y = y2;}
			else {x = x2; y = y2; end_y = y1;}

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
FillRect(game_offscreen_buffer *Buffer, int x1, int y1, int x2, int y2, uint32_t color)
{
    uint8_t *Row = (uint8_t *)Buffer->Memory + (y1 * Buffer->Pitch); // get a byte pointer to memory

    for(int  y=y1; y < y2; ++y)
    {
        uint32_t *Pixel = (uint32_t *)Row + x1; // point to the start of the row
        for(int x=x1; x < x2; ++x)
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
	DrawLine(Buffer, 500,0,500,250, WHITE); //vertical line test
	DrawLine(Buffer, 550,100, 700,100, WHITE); //horizontal line test

	DrawLine(Buffer, 1100,0, 1200,25, WHITE); //xpos, ypos shallow
	DrawLine(Buffer, 1100,0, 1200,500, WHITE); //xpos, ypos steep

	DrawLine(Buffer, 600,500, 700,475, WHITE); //xpos, yneg shallow
	DrawLine(Buffer, 600,500, 700,300, WHITE); //xpos, yneg steep

	DrawLine(Buffer, 1000, 0,  800,50, WHITE); //xneg, ypos shallow
	DrawLine(Buffer, 1000, 0,  800,400, WHITE ); //xneg, ypos steep

	DrawLine(Buffer, 1200, 700,  500, 650, WHITE); //xneg, yneg shallow
	DrawLine(Buffer, 1200, 700,  700, 150, WHITE ); //xneg, yneg steep

	//FillRect(Buffer, 0,0, 1200,500, WHITE);

	// This is called 'per-frame'
}