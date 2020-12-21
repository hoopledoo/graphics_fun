#ifndef DRAWING_H
#define DRAWING_H

// This includes definitions of the different structs
#include "game.h"
#include <vector>

#define BLUE 0x0000ff
#define GREEN 0x00ff00
#define RED 0xff0000
#define BLACK 0x000000 
#define WHITE 0xffffff
#define YELLOW 0xffff00

#define ABS(N) ((N<0)?(-N):(N))

typedef struct Point_2D
{
    real32 x;
    real32 y; 
} Point_2D;

typedef struct Point_3D
{
    real32 x;
    real32 y;
    real32 z;
} Point_3D;

typedef struct Triangle_2D
{
	Point_2D p1;
	Point_2D p2;
	Point_2D p3;
} Triangle_2D;

typedef struct Triangle_3D
{
	Point_3D p1;
	Point_3D p2;
	Point_3D p3;

} Traingle_3D;

typedef struct Mesh
{
	std::vector<Triangle_3D> tris;
} Mesh;

internal void MatrixVecMult(Point_3D *out_point, Point_3D *in_point, real32 m[4][4]);

internal void DrawPixel(game_offscreen_buffer *Buffer, real32 x, real32 y, uint32_t color);
internal void DrawLine_2D(game_offscreen_buffer *Buffer, Point_2D p1, Point_2D p2, uint32_t color);
internal void DrawTriangle_2D(game_offscreen_buffer *Buffer, Point_2D p1, Point_2D p2, Point_2D p3, uint32_t color);
internal void DrawTriangle_3D(game_offscreen_buffer *Buffer, Point_3D p1, Point_3D p2, Point_3D p3, real32 projMatrix[4][4], uint32_t color);


internal void FillRect_2D(game_offscreen_buffer *Buffer, Point_2D p1, Point_2D p2, uint32_t color);
internal void FillColor(game_offscreen_buffer *Buffer, uint32_t color);


#endif // DRAWING_H