/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#include "modeldisplay.h"
#include "model.h"

ModelDisplay::ModelDisplay(char *modelfile)
    : Display()
{
    modelmain = new Model(modelfile);
}

ModelDisplay::~ModelDisplay()
{
}

void ModelDisplay::draw()
{
    if(!drawstart())
        return;
    modelmain->draw();
    drawfinish();
}

void createDisplay(char *modelfile)
{
    if(display == NULL && modelfile != NULL)
        display = new ModelDisplay(modelfile);
}
