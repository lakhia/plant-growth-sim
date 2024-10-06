/******************************************************************************
 *    File : lowlevel.cpp
 * Descrip : Implementation file for drawing cones
 *****************************************************************************/

/* Include files */
#include "lowlevel.h"

/* Draw one vertex of cylinder */
static void drawCylVertex(float jangle, float rad, float height)
{
    GLfloat nx, ny, nz;
    GLfloat vx, vy, vz;
    float length;
    
    /* vertex position */
    vx = rad*cos(jangle);
    vy = rad*sin(jangle);
    vz = height;
    /* normal is same as vector from origin to vertex */
    nx = vx;
    ny = vy;
    nz = 0;                             /* no z component */
    /* normalize normal */
    length = sqrt(nx*nx + ny*ny);
    nx /= length;
    ny /= length;
    /* actually set vertex and normal */
    glNormal3f(nx, ny, nz);
    glVertex3f(vx, vy, vz);
}

/* Hacked cone that re-cycles code from cylinder. Normals will look wrong if 
   cone is extremely sloped.
*/
void drawCone(float rad, float rad2, float height, bool withCaps, int slices)
{
    int j;
    float jangle;

    glBegin(GL_QUAD_STRIP);
    for (j=0; j<=slices; j++)
    {
        jangle = j*twoPI/slices;         /* jangle goes from 0 to 2*PI */
        /* draw one cone segment */
        drawCylVertex(jangle, rad, 0.0);
        drawCylVertex(jangle, rad2, height);
    }
    glEnd();
    if (!withCaps)
        return;
    glBegin(GL_POLYGON);
    for (j=0; j<=slices; j++)
    {
        jangle = j*twoPI/slices;         /* jangle goes from 0 to 2*PI */
        glNormal3f(0, 0, -1);
        glVertex3f(rad*cos(jangle), rad*sin(jangle), 0);
    }
    glEnd();
    glBegin(GL_POLYGON);
    for (j=0; j<=slices; j++)
    {
        jangle = j*twoPI/slices;         /* jangle goes from 0 to 2*PI */
        glNormal3f(0, 0, 1);
        glVertex3f(rad2*cos(jangle), rad*sin(jangle), height);
    }
    glEnd();
}     
