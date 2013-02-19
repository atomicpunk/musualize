/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#include "defines.h"
#include "model.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <math.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

Model::Model(const char *filename)
{
    /* initial values */
    polygon_count = 0;
    polygons = NULL;
    scale = 1.0;
    for(int i = 0; i < 3; i++)
    {
        axismin[i] = 0;
        axismax[i] = 0;
    }

    /* prepare to parse the file */
    std::ifstream fp(filename);
    std::string line;
    std::string field;
    if(!fp.is_open())
    {
        fprintf(stderr, "Model load failure: %s\n", filename);
        return;
    }

    if(verbose)
        printf("Model: %s\n", filename);

    /* get the total polygon count (for memory preallocation) */
    while(fp.good())
    {
        std::getline(fp, line);
        std::stringstream parse(line);
        parse >> field;
        if(!field.compare("facet"))
            polygon_count++;
    }
    if(polygon_count <= 0)
    {
        fp.close();
        fprintf(stderr, "Model has no polygons: %s\n", filename);
        return;
    }
    if(verbose)
        printf("\tPOLYGONS: %d\n", polygon_count);

    /* create the memory for the polygon data and reset the stream */
    fp.clear();
    fp.seekg(std::ios::beg);
    polygons = new Polygon[polygon_count];

    /* read in every polygon */
    for(int ln = 1, i = 0, j = 0; fp.good() && (i < polygon_count); ln++)
    {
        std::getline(fp, line);
        std::stringstream parse(line);
        parse >> field;
        if(!field.compare("facet"))
        {
            parse >> field;
            parse >> polygons[i].facet[0] >> polygons[i].facet[1] >> polygons[i].facet[2];
        }
        else if(!field.compare("vertex"))
        {
            /* sanity check */
            if(j > 2)
            {
                fprintf(stderr, "found more than 3 vertexes at %s:%d\n", filename, ln);
                break;
            }

            /* pull out X, Y, Z coordinates */
            parse >> polygons[i].vertex[j][0] >> polygons[i].vertex[j][1] >> polygons[i].vertex[j][2];

            /* recalibrate the axis ranges */
            for(int k = 0; k < 3; k++)
            {
                if(polygons[i].vertex[j][k] > axismax[k])
                    axismax[k] = polygons[i].vertex[j][k];
                else if(polygons[i].vertex[j][k] < axismin[k])
                    axismin[k] = polygons[i].vertex[j][k];
            }

            /* move on to the next vertex */
            j++;
        }
        else if(!field.compare("endfacet"))
        {
            i++;
            j = 0;
        }
    }

    fp.close();

    centerObject();
    calculateInitialScale();
    calculateUnitVectors();

    if(verbose)
    {
        printf("\tXAXIS [%f, %f]\n", axismin[0], axismax[0]);
        printf("\tYAXIS [%f, %f]\n", axismin[1], axismax[1]);
        printf("\tZAXIS [%f, %f]\n", axismin[2], axismax[2]);
    }
}

Model::~Model()
{
    if(polygons)
        delete polygons;
}

void Model::centerObject()
{
    for(int i = 0; i < 3; i++)
    {
        float dx = ((axismax[i]-axismin[i])/2) - axismax[i];
        for(int j = 0; j < 3; j++)
            for(int k = 0; k < polygon_count; k++)
                polygons[k].vertex[j][i] += dx;
    }
}

void Model::calculateUnitVectors()
{
    for(int i = 0; i < polygon_count; i++)
    {
        float a = pow(polygons[i].facet[0], 2) +
                  pow(polygons[i].facet[1], 2) +
                  pow(polygons[i].facet[2], 2);
        if(a > 0)
        {
            a = pow((1 / a), 0.5);
            for(int j = 0; j < 3; j++)
                polygons[i].facet[j] *= a;
        }
    }
}

void Model::calculateInitialScale()
{
    float mlen = axismax[0];
    for(int i = 0; i < 3; i++)
    {
        if(axismax[i] > mlen)
            mlen = axismax[i];
        if(-1*axismin[i] > mlen)
            mlen = -1*axismin[i];
    }
    if(mlen <= 0)
    {
        fprintf(stderr, "Model has a negative or zero scale\n");
        return;
    }
    scale = 1/mlen;
}

void Model::draw()
{
    for(int i = 0; i < polygon_count; i++)
    {
        glBegin(GL_POLYGON);
        glNormal3f(polygons[i].facet[0], polygons[i].facet[1], polygons[i].facet[2]);
        for(int j = 0; j < 3; j++)
        {
            glVertex3f(polygons[i].vertex[j][0]*scale,
                       polygons[i].vertex[j][1]*scale,
                       polygons[i].vertex[j][2]*scale);
        }
        glEnd();
    }
}
