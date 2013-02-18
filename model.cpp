/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#include "model.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <string>
#include <fstream>

Model::Model(const char *filename)
{
    std::ifstream fp(filename);
    std::string line;

    if(!fp.is_open())
    {
        fprintf(stderr, "Model load failure: %s\n", filename);
        return;
    }

    int pc = 0;
    while(fp.good())
    {
        std::getline(fp, line);
        line.erase(0, line.find_first_not_of(" \t\n"));
        line.erase(line.find_last_not_of(" \t\n")+1, std::string::npos);
        if(!line.compare("endfacet"))
            pc++;
    }
    printf("Polygons = %d\n", pc);
    fp.close();
}

Model::~Model()
{
}
