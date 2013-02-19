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
#include "model.h"

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
    static void analyzerUpdate(float *spectrum, unsigned char *colors, int size);

    void changeScale(float val);
    void changeRotation(float xval, float yval);
    void update(float *spectrum, unsigned char *colors, int size);
    void draw();
    bool paused;
    bool redraw;
    float winWidth;
    float winHeight;

private:
    int window;
    float rotation[2];
    Model *modelmain;
};

extern Display *display;

#endif //DISPLAY_H

