#ifndef DISPLAY_H
#define DISPLAY_H

#include "GL/freeglut.h"

class Display {
public:
    Display();
    ~Display();

    static Display* create();
    void update(int *spectrum, unsigned char *colors, int size);
private:
    int ignore;
};

#endif //DISPLAY_H
