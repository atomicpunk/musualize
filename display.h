/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "GL/freeglut.h"
#include "defines.h"

class MouseState {
public:
    MouseState()
    {
        for(int i = 0; i < 5; i++)
            buttons[i] = false;
        px = 0;
        py = 0;
        dx = 0;
        dy = 0;
    };
    ~MouseState() {};
    bool buttons[5];
    int px;
    int py;
    int dx;
    int dy;
};

class Display {
public:
    Display();
    ~Display();

    static void create();
    static void doRedraw();

    void changeScale(float val);
    void changeRotation(float xval, float yval);
    virtual void drawContent();
    void draw();
    bool paused;
    bool redraw;
    float winWidth;
    float winHeight;

protected:
    void drawPolygon(float, float, float,
         float, float, float, float, float, float);

private:
    int window;
    float rotation[2];
};

extern Display *display;

#endif //DISPLAY_H

