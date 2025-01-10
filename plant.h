/******************************************************************************
 *    File : plant.h
  * Descrip : Header file for plant grower.
 *****************************************************************************/

#pragma once

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

/* Constants */
#define SPIN 0.0025                     /* Speed of right/left spin */
#define SPEED 0.005                     /* Speed of forward/backward mov */
#define INIT_SIZE 0.4                   /* Initial size of plant */
#define TIME_INCR 0.00005f              /* Growth rate */
#define TREE_DEPTH 1                    /* Depth of leaves in L-system */
#define SIGMOID_GROWTH 0                /* Set to 1 to use Sigmoidal growth */
#define STOCASTIC_PLANT 0               /* Set to 1 to make stocastic plant */
#define STATE_SIZE 10000                /* Max size of state tree */
#define CONE_APPROX 3                   /* Higher values approx cone better */
#define RAND_DIST uniform               /* Set to "uniform" or "guassian" */
#define GREEN 0, 1, 0                   /* Green color */
#define GREEN_LEAVES 0                  /* Are leaves green or grow color? */

/* Tree parameters */
#define AZIM_SPIN 5                     /* Azimuth spin per apex node */
#define TURN_SPIN 0                     /* Turning of branch per apex node */
#define BRANCH_PER_APEX 2               /* Number of leaves per apex node */
#define LEAF_OUTWARD_ANGLE 38           /* Angle leaf makes with apex */
#define LEAF_TO_TWIG_RATIO 50           /* Leaf is half the size of twig */
#define BIG_AZIM_SPIN 55                /* Azimuth spin per apex node */
#define BIG_TURN_SPIN 0                 /* Turning of branch per apex node */
#define BIG_BRANCH_PER_APEX 2           /* Number of leaves per apex node */
#define BIG_LEAF_OUTWARD_ANGLE 65       /* Angle leaf makes with apex */

/* Starting position is different based on different renderers */
#define STARTX 0                        /* right */
#define STARTY 5                        /* depth */
#define STARTZ -5                       /* height */

/* Include files */
#include "lowlevel.h"
#include "bitmap.h"
#include <time.h>
#include <fstream>
#include <iostream>

/* Types */
typedef struct state {
    float size;
    float deg;
    float azimuth;
    float mytime;
    int rule;
    int child, sibling;
} state;

/* Variables local to this file */
static float time_cur = 1;              /* Current time in animation */
static float time_step = 0;             /* Time step */
static bool make_movie = false;         /* If true, save each frame */
static state states[STATE_SIZE];        /* State hierarchy */
static int nextFree = 0;                /* Next free state available */
static bool quit = false;               /* Quit if true */

/* current view matrices */
static GLfloat curview[16];
GLfloat start[16];

/* window size */
static int winx = 500;
static int winy = 500;

/* Colors */
const GLfloat GREY[4] = {.25, .45, .35, 1};
const GLfloat color_tip[] = {GREEN};
const GLfloat color_diff[] = {0.6, -0.7, 0};

/* Velocity & rate of spin variables */
float zpos = 0.0;                       /* Forward velocity */
float yspin = 0.0;                      /* Rotation left or right */
float xspin = 0.0;                      /* Rotation up or down */

/* Mouse related variables */
int xOldMouse = -1;                     /* Previous mouse X-coordinate */
int yOldMouse = -1;                     /* Previous mouse Y-coordinate */
bool mouseDown = 0;                     /* True if middle button down */

/* Constants */
#define NONE -1                         /* Same as NULL for state tree */

/* Prototypes */
void drawTwig(float mytime, float size_bot, float size, float deg,
              float azimuth, int &mystate);
void drawBigTwig(float mytime, float size_bot, float size, float deg,
                 float azimuth, int &mystate);
