/******************************************************************************
 *    File : lowlevel.h
 * Descrip : Header file for implementation file
 *****************************************************************************/

#pragma once

/* Include files */
#include <math.h>                       /* Need math functions */
#include <GLUT/glut.h>                  /* OpenGL routines */
#include <stdio.h>                      /* For file operations */
#include <string.h>                     /* String operations */
#include <stdlib.h>

/* Constants */
#define mapImageWidth	512
#define mapImageHeight	256
#define texmax 		3
#define twoPI 		6.283185307179586476925286766559f
#define DEG_2_RAD   M_PI/180

/* Macros */
#ifndef MIN
# define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
# define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

/* Types */
typedef struct vertex {
    float x, y, z;
} vertex;
        
/* Procedure prototypes */
void drawCone(float rad, float rad2, float height, bool withCaps, int slices);
