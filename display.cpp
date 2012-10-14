#include <stdio.h>
#include "display.h"
#include "GL/gl.h"

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920
#define WINDOW_X 0
#define WINDOW_Y 0
#define LINESIZE 4
#define SMAX 200.0
#define MAG(n) ((((float)(n)/SMAX)>1)?1:((float)(n)/SMAX))

void Display::update(int *spectrum, int size)
{
    static int y = 0;
    int i;
    float n;
    float dy = 1-(((float)y/(float)WINDOW_HEIGHT)*2);
    float dx = 2.0/(float)size;

    glBegin(GL_LINES);
    for(i = 0; i < size; i++)
    {
        n = MAG(spectrum[i]);
        glColor3f(n,n,n);
        glVertex2f((i*dx)-1, dy);
        glVertex2f(((i+1)*dx)-1, dy);
    }
    glEnd();
    glFlush();
    glutMainLoopEvent();

    y+=LINESIZE;
    if(y > WINDOW_HEIGHT) y = 0;
}

Display::Display()
{
    glutInitDisplayMode(GLUT_SINGLE);
//    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
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
