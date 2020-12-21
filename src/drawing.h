#ifndef DRAWING_H
#define DRAWING_H

// This includes definitions of the different structs
#include "game.h"

#define BLUE 0x0000ff
#define GREEN 0x00ff00
#define RED 0xff0000
#define BLACK 0x000000 
#define WHITE 0xffffff
#define YELLOW 0xffff00

#define ABS(N) ((N<0)?(-N):(N))

typedef struct Point_2D
{
    int x;
    int y; 
} Point_2D;

typedef struct Point_3D
{
    int x;
    int y;
    int z;
} Point_3D;

#endif // DRAWING_H