/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#include "musicdisplay.h"
#include "analyzer.h"

extern Analyzer *analyzer;

MusicDisplay::MusicDisplay()
    : Display()
{
}

MusicDisplay::~MusicDisplay()
{
}

void MusicDisplay::drawContent()
{
    if(analyzer == NULL)
        return;

    float *s = analyzer->spectrum;
#ifdef TONE_HISTORY
    float dx = 2.0/(float)analyzer->numtones;
    float dy = 2.0/TONE_HISTORY;
    int N = analyzer->numtones;

    for(int i = 0; i < TONE_HISTORY-1; i++)
    {
        for(int j = 0; j < N-1; j++)
        {
            float x1 = 1-(i*dy);
            float y1 = 1-(j*dx);
            float z1 = s[j+(i*N)]-0.3;

            float x2 = 1-(i*dy);
            float y2 = 1-((j+1)*dx);
            float z2 = s[j+1+(i*N)]-0.3;

            float x3 = 1-((i+1)*dy);
            float y3 = 1-(j*dx);
            float z3 = s[j+((i+1)*N)]-0.3;

            float x4 = 1-((i+1)*dy);
            float y4 = 1-((j+1)*dx);
            float z4 = s[j+1+((i+1)*N)]-0.3;

            drawPolygon(x3, y3, z3, x1, y1, z1, x2, y2, z2);
            drawPolygon(x2, y2, z2, x4, y4, z4, x3, y3, z3);
        }
    }
#else
    int cols = (analyzer->tdiv)*12;
    int rows = (analyzer->numtones)/cols;
    float dx = 2.0/(float)cols;
    float dy = 2.0/(float)rows;

    for(int i = 0; i < rows-1; i++)
    {
        for(int j = 0; j < cols-1; j++)
        {
            int a = (i * cols) + j;
            int b = (i * cols) + j + 1;
            int c = ((i + 1) * cols) + j;
            int d = ((i + 1) * cols) + j + 1;

            if(d >= analyzer->numtones)
                return;

            float x1 = (j*dx)-1;
            float y1 = 1-(i*dy);
            float z1 = s[a]-0.3;

            float x2 = ((j+1)*dx)-1;
            float y2 = 1-(i*dy);
            float z2 = s[b]-0.3;

            float x3 = (j*dx)-1;
            float y3 = 1-((i+1)*dy);
            float z3 = s[c]-0.3;

            float x4 = ((j+1)*dx)-1;
            float y4 = 1-((i+1)*dy);
            float z4 = s[d]-0.3;

            drawPolygon(x3, y3, z3, x1, y1, z1, x2, y2, z2);
            drawPolygon(x2, y2, z2, x4, y4, z4, x3, y3, z3);
        }
    }
#endif
}

void createDisplay()
{
    if(display == NULL)
        display = new MusicDisplay();
}
