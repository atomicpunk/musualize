#include <stdio.h>
#include "display.h"
#include "GL/gl.h"
#include "GL/glext.h"
#include "defines.h"
#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920
#define WINDOW_X 0
#define WINDOW_Y 0
#define LINEY (((float)(LINESIZE)/(float)WINDOW_HEIGHT)-1.0)
#define RPIX(c, n) ((((c)>>2)&0x1)?(n):0)
#define GPIX(c, n) ((((c)>>1)&0x1)?(n):0)
#define BPIX(c, n) (((c)&0x1)?(n):0)

void Display::update(float *spectrum, unsigned char *colors, int size)
{
    unsigned char c;
    int i, j;
    float n, dx = 2.0/(float)size;

    if(ignore > 0)
    {
        ignore--;
        return;
    }
    glRasterPos2d(-1, LINEY);
    glCopyPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR);

    glBegin(GL_LINES);
    if(colors[0] == 23)
    {
        size = 0;
        glColor3f(1, 1, 1);
        glVertex2f(-1, -1);
        glVertex2f(1, -1);
    }

    for(i = 0; i < size; i++)
    {
        n = spectrum[i];
        c = colors[i];
        glColor3f(RPIX(c, n),GPIX(c, n), BPIX(c, n));
        glVertex2f((i*dx)-1, -1);
        glVertex2f(((i+1)*dx)-1, -1);
    }
    glEnd();
    glFlush();
    glutMainLoopEvent();
}

Display::Display()
{
    ignore = 100;
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowPosition(WINDOW_X, WINDOW_Y);
    glutCreateWindow("Musualizer Spectrum");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(LINESIZE*2);
    glutFullScreen();
    glutMainLoopEvent();
}

Display::~Display()
{

}

Display* Display::create()
{
    return new Display();
}
