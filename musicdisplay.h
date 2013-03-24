/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifndef MUSICDISPLAY_H
#define MUSICDISPLAY_H

#include "display.h"

class MusicDisplay : public Display {
public:
    MusicDisplay();
    ~MusicDisplay();
    void draw();
private:
    void drawSpectrum();
};

void createDisplay();

#endif //MUSICDISPLAY_H
