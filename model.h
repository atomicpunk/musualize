/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifndef MODEL_H
#define MODEL_H

#include <string>

class Polygon {
public:
    Polygon() {};
    ~Polygon() {};
    float facet[3];
    float vertex[3][3];
};

class Model {
public:
    Model(const char* filename);
    ~Model();

    void draw();
    float scale;
private:
    void centerObject();
    void calculateInitialScale();
    void calculateUnitVectors();
    float axismin[3];
    float axismax[3];
    int polygon_count;
    Polygon *polygons;
};

#endif //MODEL_H
