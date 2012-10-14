#include <stdio.h>
#include "display.h"
#include "GL/gl.h"
#include "GL/glext.h"

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920
#define WINDOW_X 0
#define WINDOW_Y 0
#define LINESIZE 4
#define LINEY (((float)LINESIZE/(float)WINDOW_HEIGHT)-1)
#define SMAX 200.0
#define MAG(n) ((((float)(n)/SMAX)>1)?1:((float)(n)/SMAX))

void Display::update(int *spectrum, int size)
{
    int i;
    float n, dx = 2.0/(float)size;

    if(ignore > 0)
    {
        ignore--;
        return;
    }
    glRasterPos2d(-1, LINEY);
    glCopyPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR);

    glBegin(GL_LINES);
    for(i = 0; i < size; i++)
    {
        n = MAG(spectrum[i]);
        glColor3f(n,n,n);
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
    glLineWidth(LINESIZE);
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
