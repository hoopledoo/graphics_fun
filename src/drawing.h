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
#define PURPLE 0xf800f0

#define ABS(N) ((N<0)?(-N):(N))
#define ROUND_INT(x) ((((int)(x))<((int)((x)+0.5f)))?((int)(x)):((int)((x)+0.5f)))

typedef struct Point_2D_Real
{
    real32 x;
    real32 y; 
} Point_2D_Real;

typedef struct Point_2D_Int
{
    int32_t x;
    int32_t y; 
} Point_2D_Int;

typedef struct Point_3D
{
    real32 x;
    real32 y;
    real32 z;
} Point_3D;

typedef struct Vec3D
{
    real32 x;
    real32 y;
    real32 z;
} Vec3D;

typedef struct Triangle_2D_Real
{
	Point_2D_Real p1;
	Point_2D_Real p2;
	Point_2D_Real p3;
} Triangle_2D_Real;

typedef struct Triangle_2D_Int
{
	Point_2D_Int p1;
	Point_2D_Int p2;
	Point_2D_Int p3;
} Triangle_2D_Int;

typedef struct Triangle_3D
{
	Point_3D p1;
	Point_3D p2;
	Point_3D p3;

} Triangle_3D;

typedef struct Mesh
{
	std::vector<Triangle_3D> tris;
	Point_3D offset = {0};
	Point_3D scale = {0};
} Mesh;

internal void MatrixVecMult(Point_3D *out_point, Point_3D *in_point, real32 m[4][4]);

internal void DrawPixel(game_offscreen_buffer *Buffer, int32_t x, int32_t y, uint32_t color);
internal void DrawPixel(game_offscreen_buffer *Buffer, Point_2D_Int p, uint32_t color);

internal void DrawLine_2D(game_offscreen_buffer *Buffer, Point_2D_Real p1, Point_2D_Real p2, uint32_t color);
internal void DrawLine_2D(game_offscreen_buffer *Buffer, Point_2D_Int p1, Point_2D_Int p2, uint32_t color);

internal void DrawTriangle_2D(game_offscreen_buffer *Buffer, Point_2D_Real p1, Point_2D_Real p2, Point_2D_Real p3, uint32_t color);
internal void DrawTriangle_2D(game_offscreen_buffer *Buffer, Point_2D_Int p1, Point_2D_Int p2, Point_2D_Int p3, uint32_t color);

internal void DrawTriangle_3D(game_offscreen_buffer *Buffer, Point_3D p1, Point_3D p2, Point_3D p3, real32 projMatrix[4][4], uint32_t color);

internal void FillTriangle_2D(game_offscreen_buffer *Buffer, Point_2D_Real p1, Point_2D_Real p2, Point_2D_Real p3, uint32_t color);
internal void FillTriangle_2D(game_offscreen_buffer *Buffer, Point_2D_Int p1, Point_2D_Int p2, Point_2D_Int p3, uint32_t color);

internal void FillFlatTopBresenham(game_offscreen_buffer *Buffer, Point_2D_Int start, Point_2D_Int end1, Point_2D_Int end2, uint32_t color);
internal void FillFlatTopBresenham(game_offscreen_buffer *Buffer, Point_2D_Real start, Point_2D_Real end1, Point_2D_Real end2, uint32_t color);
internal void FillFlatBottomBresenham(game_offscreen_buffer *Buffer, Point_2D_Int start, Point_2D_Int end1, Point_2D_Int end2, uint32_t color);
internal void FillFlatBottomBresenham(game_offscreen_buffer *Buffer, Point_2D_Real start, Point_2D_Real end1, Point_2D_Real end2, uint32_t color);

internal void FillRect_2D(game_offscreen_buffer *Buffer, Point_2D_Real p1, Point_2D_Real p2, uint32_t color);
internal void FillRect_2D(game_offscreen_buffer *Buffer, Point_2D_Int p1, Point_2D_Int p2, uint32_t color);

internal void FillColor(game_offscreen_buffer *Buffer, uint32_t color);


#endif // DRAWING_H