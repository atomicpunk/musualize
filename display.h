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

