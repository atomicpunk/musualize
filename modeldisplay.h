/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifndef MODELDISPLAY_H
#define MODELDISPLAY_H

#include "display.h"
#include "model.h"

class ModelDisplay : public Display {
public:
    ModelDisplay(char *);
    ~ModelDisplay();
    void draw();
private:
    Model *modelmain;
};

void createDisplay(char *modelfile = NULL);

#endif //MODELDISPLAY_H

