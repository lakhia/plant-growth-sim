/******************************************************************************
 *    File : plant.cpp
 * Descrip : Implementation file for plant growing
 *****************************************************************************/

/* Include files */
#include "plant.h"
#include <math.h>                       /* Need math functions */
#include <GLUT/glut.h>                  /* OpenGL routines */
#include <stdio.h>                      /* For file operations */
#include <string.h>                     /* String operations */
#include <stdlib.h>

void initLighting()
{
    static float light1_ambient[] =  {0.46, 0.46, 0.46, 1.0};
    static float light1_diffuse[] =  {1.0,  1.0,  1.0,  1.0};
    static float light1_specular[] = {2.0,  2.0,  2.0,  1.0};
    static float mat_shininess[] = {100.0};

    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glEnable(GL_LIGHT1);

    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    /* Have color set material properties as well */
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_AMBIENT, GREY);
}

void setLight()
{
    static float light0_position[] = {0.0, -1.0, 1.0, 0.0};
    static float light1_position[] = {-0.4, 1.0, 0.5, 0.0};

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
}

/* Capture a single frame */
void capture()
{
    static unsigned char *buffer = new unsigned char[3*1500*1500];
    static unsigned int frame_cap = 0;
    char filename[100];
    sprintf(filename, "frame%05d.bmp", frame_cap++);
    glReadPixels(0, 0, winx, winy, GL_RGB, GL_BYTE, buffer);
    writeBMP(filename, winx, winy, buffer);
}

/* Save state tree */
void save_state(const char* filename)
{
    std::ofstream out(filename, std::ios::out|std::ios::binary);
    out.write((char *)&curview, sizeof(curview)); /* write view matrix */
    out.write((char *)&time_cur, sizeof(time_cur)); /* write cur time */
    out.write((char *)&nextFree, sizeof(nextFree)); /* write size of state tree */
    out.write((char *)&states, sizeof(state)*nextFree); /* write state tree */
}

/* Load state tree */
void load_state(const char* filename)
{
    std::ifstream in(filename, std::ios::in|std::ios::binary);
    in.read((char *)&start, sizeof(start)); /* read view matrix */
    in.read((char *)&time_cur, sizeof(time_cur)); /* read cur time */
    in.read((char *)&nextFree, sizeof(nextFree)); /* read size of state tree */
    in.read((char *)&states, sizeof(state)*nextFree); /* read state tree */
    memcpy(curview, start, sizeof(start));
}

/* Growth function for the each component */
float growth(float mytime)
{
#if SIGMOID_GROWTH == 1
    return 1/(1 + exp(3-mytime));
#else
    return fmax(log(mytime*4)/4, 0.0);
#endif
}

/* Function that returns a uniform random variable */
float uniform(float mu, float sigma)
{
    return mu - sigma/2 + (rand() * sigma/RAND_MAX);
}

/* Box Muller method to calculating guassian distribution */
float guassian(float m, float s)
{
    double x1, x2, w, y1;
    static double y2;
    static int use_last = 0;
 
    if (use_last)                       /* use value from previous call */
    {
        y1 = y2;
        use_last = 0;
    }
    else
    {
        do {
            x1 = 2.0 * (rand()/RAND_MAX) - 1.0;
            x2 = 2.0 * (rand()/RAND_MAX) - 1.0;
            w = x1 * x1 + x2 * x2;
        } while ( w >= 1.0 || w == 0.0);
 
        w = sqrt( (-2.0 * log( w ) ) / w );
        y1 = x1 * w;
        y2 = x2 * w;
        use_last = 1;
    }

    return( m + y1 * s );
}

/* Retrieve state from state tree */
inline int nextState(int &mystate, float &size, float &deg, float &azimuth,
                      float &mytime)
{
    if (mystate != NONE)
    {
        size = states[mystate].size;
        deg = states[mystate].deg;
        azimuth = states[mystate].azimuth;
        mytime += states[mystate].mytime;
    }
    else if (nextFree >= STATE_SIZE)
        exit(1);
    else {
        mystate = ++nextFree;
#if STOCASTIC_PLANT == 1
        states[mystate].size = RAND_DIST(size, size/3);
        states[mystate].deg = RAND_DIST(deg, 20);
        states[mystate].azimuth = RAND_DIST(azimuth, 180);
        states[mystate].mytime = RAND_DIST(-0.25, 0.5);
        states[mystate].rule = (int)RAND_DIST(2.5, 2); /* 1,2,3 */

        // Update passed parameters
        size = states[mystate].size;
        deg = states[mystate].deg;
        azimuth = states[mystate].azimuth;
        mytime = states[mystate].mytime;
#else
        states[mystate].size = size;
        states[mystate].deg = deg;
        states[mystate].azimuth = azimuth;
        states[mystate].mytime = 0;
        states[mystate].rule = 2;
#endif
        states[mystate].child = NONE;
        states[mystate].sibling = NONE;
    }
    return states[mystate].rule;
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClearColor(1.0, 1.0, 1.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    initLighting();

    /* Set up initial state */
    srand(time(0));                   /* use time as seed for randomization */
    states[0].child = NONE;
    states[0].sibling = NONE;

    /* Anti-aliasing hints */
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    /* initialize view and save */
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -15.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, curview);
    glGetFloatv(GL_MODELVIEW_MATRIX, start);
}

void drawLeaf(float mytime, float size_bot, float deg, float azimuth,
              int &mystate)
{
    /* Base case */
    if (size_bot <= 0 || mytime < 0)
        return;

    static float size;
    static float factor;
    static int rule;
    size = INIT_SIZE*LEAF_TO_TWIG_RATIO;
    rule = nextState(mystate, size, deg, azimuth, mytime);

    factor = growth(mytime);
    size *= factor;
    deg *= factor;

    if (rule == 2)
    {
        glPushMatrix();
        glRotatef(TURN_SPIN, 0,1,0);
        glRotatef(azimuth, 0,0,1);
        glRotatef(deg, 0,1,0);
#if GREEN_LEAVES==1
        glColor3f(GREEN);
#else
        glColor3f(color_tip[0]+factor*color_diff[0],
                  color_tip[1]+factor*color_diff[1],
                  color_tip[2]+factor*color_diff[2]);
#endif
        drawCone(size_bot, 0.02*size_bot, size*size_bot, false, CONE_APPROX);
        glPopMatrix();
    }
#if TREE_DEPTH == 20                    /* This case doesn't work */
    else if (rule == 1)
    {
        glPushMatrix();
        drawTwig(mytime-0.5, size_bot, size_bot, deg, azimuth,
                 states[mystate].child);
        glPopMatrix();
    }        
    else
    {
        glPushMatrix();
        drawBigTwig(mytime-0.5, size_bot, INIT_SIZE, deg, azimuth,
                    states[mystate].child);
        glPopMatrix();
    }
#endif
}

void drawTwig(float mytime, float size_bot, float size, float deg,
             float azimuth, int &mystate)
{
    // Base case
    if (mytime < 0)
        return;

    // Temporary vars are static to save stack space
    static int rule;
    static int spin;
    static float size_top;
    static float factor;
    static state* sib_state;

    rule = nextState(mystate, size, deg, azimuth, mytime)*BRANCH_PER_APEX;
    if (rule > 0)
        spin = 360/rule;
    factor = growth(mytime-0.5);
    size_top = INIT_SIZE * factor;
    deg *= factor;

    /* Render twig */
    glColor3f(color_tip[0]+factor*color_diff[0],
              color_tip[1]+factor*color_diff[1],
              color_tip[2]+factor*color_diff[2]);
    glRotatef(azimuth, 0,0,1);
    glRotatef(deg, 0,1,0);
    drawCone(size_bot, size_top, size_bot*10, false, CONE_APPROX);
    glTranslatef(0, 0, size_bot*10);

    /* Render sibling leaves */
    sib_state = &states[mystate];
    for (int i=0; i<rule; i++) {
        drawLeaf(mytime, size_top, LEAF_OUTWARD_ANGLE, azimuth,
                 sib_state->sibling);
        sib_state = &states[sib_state->sibling];
        azimuth += spin;
    }

    /* Undo rotational transformation */
    glRotatef(-deg+TURN_SPIN, 0,1,0);
    glRotatef(-azimuth+AZIM_SPIN, 0,0,1);

    /* Recurse */
    drawTwig(mytime-0.5, size_top, size, deg, azimuth,
             states[mystate].child);
}

void drawBigTwig(float mytime, float size_bot, float size, float deg,
                float azimuth, int &mystate)
{
    // Base case
    if (mytime < 0)
        return;

    // Temporary vars are static to save stack space
    static int rule;
    static float spin;
    static float size_top;
    static float factor;
    static state* sib_state;

    rule = nextState(mystate, size, deg, azimuth, mytime)*BIG_BRANCH_PER_APEX;
    if (rule > 0)
        spin = 360/rule;
    factor = growth(mytime-0.5);
    size_top = INIT_SIZE * factor;

    /* Render twig */
    glColor3f(color_tip[0]+factor*color_diff[0],
              color_tip[1]+factor*color_diff[1],
              color_tip[2]+factor*color_diff[2]);
    glRotatef(azimuth, 0,0,1);
    glRotatef(deg, 0,1,0);
    drawCone(size_bot, size_top, size_bot*10, false, CONE_APPROX);
    glTranslatef(0, 0, size_bot*10);

    /* Render sibling leaves */
    sib_state = &states[mystate];
    for (int i=0; i<rule; i++) {
        glPushMatrix();
        drawTwig(mytime, size_top, INIT_SIZE, BIG_LEAF_OUTWARD_ANGLE,
                 azimuth, sib_state->sibling);
        sib_state = &states[sib_state->sibling];
        azimuth += spin;
        glPopMatrix();
    }
    glRotatef(-deg+BIG_TURN_SPIN, 0,1,0);
    glRotatef(-azimuth+BIG_AZIM_SPIN, 0,0,1);

    /* Recurse */
    drawBigTwig(mytime-0.5, size_top, size, deg,
                azimuth, states[mystate].child);
}

void display()
{
    static int root = 0;                /* Root of state tree */

    /* Quit gracefully */
    if (quit)
        exit(0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Move the image plane, keeping the frustum angle */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(-winx/1000.0, winx/1000.0,
              -winy/1000.0, winy/1000.0, 1.0, 1000.0);

    /* Postion camera */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glRotatef(yspin, 0, 1, 0);          /* Spin left-right */
    glRotatef(xspin, 1, 0, 0);          /* Spin up-down */
    glTranslatef(0.0, 0.0, zpos);       /* Move forward or backward */

    glMultMatrixf(curview);
    glGetFloatv(GL_MODELVIEW_MATRIX, curview);

    setLight();                         /* Position lights after camera
                                           transformation */

    /* Take one time step, but don't grow negative */
    if (time_cur > 0 || time_step > 0)
    	time_cur += time_step;

    /* Render plant */
    float size_bot = INIT_SIZE * growth(time_cur);

    glRotatef(-90, 1,0,0);
    glTranslatef(STARTX, STARTY, STARTZ);
#if TREE_DEPTH==2
    drawBigTwig(time_cur, size_bot, INIT_SIZE, 0, 0, root);
#else
    drawTwig(time_cur, size_bot, INIT_SIZE, 0, 0, root);
#endif

    /* Are we rendering for a movie? */
    if (make_movie)
        capture();

    glPopMatrix();
    glutSwapBuffers();
}

/* Function called when mouse is moved while one of the buttons is held down */
void motion(int x, int y)
{
    if (!mouseDown)
        return;                         /* Middle button not down, do nada */
    if (xOldMouse != -1)
    {
        yspin -= (x - xOldMouse)*SPIN/1900.0;
        xspin -= (y - yOldMouse)*SPIN/1900.0;
    }
    xOldMouse = x;
    yOldMouse = y;
}

/* Reshape of window calls this function */
void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    
    winx = w;
    winy = h;
    
    glFrustum(-winx/1000.0, winx/1000.0,
              -winy/1000.0, winy/1000.0, 1.0, 100.0);
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    glutPostRedisplay();
}

/* When keyboard is used, this function is called */
void keyboard(unsigned char key, int x, int y)
{
    xOldMouse = -1;                     /* Forget old mouse position */

    switch(key)
    {
        case 'f':
        case '8':
            zpos += SPEED;              /* go forward */
            glutPostRedisplay();
            break;
        case 'g':
            time_step += TIME_INCR;     /* speed up growth */
            break;
        case 'G':
            time_step -= TIME_INCR;     /* slow growth */
            break;
        case 'h':
            time_step = 0;              /* stop growth */
            break;
        case '2':
        case 'b':
            zpos -= SPEED;              /* go backward */
            glutPostRedisplay();
            break;
        case 'd':
        case '+':
            xspin += SPIN;              /* spin down */
            glutPostRedisplay();
            break; 
        case 'u':
        case '-':
            xspin -= SPIN;              /* spin up */
            glutPostRedisplay();
            break;
        case '6':
        case 'r':
            yspin += SPIN;              /* spin right */
            glutPostRedisplay();
            break;
        case '4':
        case 'l':
            yspin -= SPIN;              /* spin left */
            glutPostRedisplay();
            break;
        case '5':
        case 'k':                       /* brake */
            xspin = 0.0;
            yspin = 0.0;
            zpos = 0.0;
            glutPostRedisplay();
            break;
        case 's':                       /* go back to starting position */
            memcpy(curview, start, sizeof(start));
            break;
        case 'q':
            quit = true;                /* quit gracefully */
            break;
        case '7':
            zpos += SPEED;              /* go forward */
            yspin -= SPIN;              /* spin left */
            glutPostRedisplay();
            break;
        case '9':
            zpos += SPEED;              /* go forward */
            yspin += SPIN;              /* spin right */
            glutPostRedisplay();
            break;
        case '1':
            zpos -= SPEED;              /* go backward */
            yspin -= SPIN;              /* spin left */
            glutPostRedisplay();
            break;
        case '3':
            zpos -= SPEED;              /* go backward */
            yspin += SPIN;              /* spin right */
            glutPostRedisplay();
            break;
        case 'c':
            capture();
            break;
        case 'm':
            make_movie = !make_movie;
            break;
        case 'S':
            save_state("out.state");
            break;
        case 'L':
            load_state("out.state");
            break;
        default:
            break;
    }
}

void idle()
{
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
            {
                zpos += SPEED;          /* go forward */
                glutPostRedisplay();
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
            {
                zpos -= SPEED;          /* go backward */
                glutPostRedisplay();
            }
            break;
        case GLUT_MIDDLE_BUTTON:
            if (state == GLUT_UP)
            {
                mouseDown = 0;
                xOldMouse = -1;         /* Forget old mouse position */
            }
            else if (state == GLUT_DOWN)
                mouseDown = 1;
        default:
            break;
    }
}

int main(int argc, char** argv)
{
/*
    cerr << "The navigation controls are:" << endl;
    cerr << "  Forward:          f or 8 or left mouse click" << endl;
    cerr << "  Backward:         b or 2 or right mouse click" << endl;
    cerr << "  Right spin:       r or 6 or middle mouse dragged right" << endl;
    cerr << "  Left spin:        l or 4 or middle mouse dragged left" << endl;
    cerr << "  Upward spin:      u or - or middle mouse dragged up" << endl;
    cerr << "  Downward spin:    d or + or middle mouse dragged down" << endl;
    cerr << "  Forward & right:       9" << endl;
    cerr << "  Forward & left:        7" << endl;
    cerr << "  Backward & right:      3" << endl;
    cerr << "  Backward & left:       1" << endl;
    cerr << "  Upward & right:             middle mouse dragged up & right" << endl;
    cerr << "  Upward & left:              middle mouse dragged up & left" << endl;
    cerr << "  Downward & right:           middle mouse dragged down & right" << endl;
    cerr << "  Downward & left:            middle mouse dragged down & left" << endl;
    cerr << "  braKe:            k or 5" << endl;
    cerr << "  Start over:       s" << endl;
    cerr << "  Quit:             q" << endl;
    */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winx, winy);
    glutInitWindowPosition(100, 100);
    glutIdleFunc(idle);
    glutCreateWindow(argv[0]);
    init();
    if (argc > 1)
        load_state(argv[1]);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(motion);             /* Called when mouse button down */
    glutMainLoop();
    return(0);
}
