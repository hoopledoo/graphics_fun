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

// This is temporary, TODO: we want to better handle window sizing later
#define WIDTH 1280
#define HEIGHT 720

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

#endif // DRAWING_H