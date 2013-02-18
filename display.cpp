/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

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

bool paused = false;

void Display::update(float *spectrum, unsigned char *colors, int size)
{
    if(paused)
    {
        glutMainLoopEvent();
        return;
    }

    unsigned char c;
    int i, j;
    float n, dx = 2.0/(float)size;

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

static void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case 'p': paused = !paused; break;
    case 'g': primary_color = GREEN; break;
    case 'b': primary_color = BLUE; break;
    case 'm': primary_color = MAGENTA; break;
    case 'w': primary_color = WHITE; break;
    case 'c': primary_color = CYAN; break;
    case 'r': primary_color = RED; break;
    case 'y': primary_color = YELLOW; break;
    case 'G': highlight_color = GREEN; break;
    case 'B': highlight_color = BLUE; break;
    case 'M': highlight_color = MAGENTA; break;
    case 'W': highlight_color = WHITE; break;
    case 'C': highlight_color = CYAN; break;
    case 'R': highlight_color = RED; break;
    case 'Y': highlight_color = YELLOW; break;
    default:
        printf("KEY = %c, %x\n", key, key);
    }
}

Display::Display()
{
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowPosition(WINDOW_X, WINDOW_Y);
    glutCreateWindow("Musualizer Spectrum");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(LINESIZE*2);
    glutFullScreen();
    glutKeyboardFunc(keyboard);
    glutMainLoopEvent();
}

Display::~Display()
{

}

Display* Display::create()
{
    return new Display();
}
