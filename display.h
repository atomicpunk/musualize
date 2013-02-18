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

class Display {
public:
    Display();
    ~Display();

    static Display* create();
    void update(float *spectrum, unsigned char *colors, int size);
};

#endif //DISPLAY_H

