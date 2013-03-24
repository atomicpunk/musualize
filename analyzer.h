/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifndef ANALYZER_H
#define ANALYZER_H

#include <time.h>
#include <sys/types.h>

class Tone {
public:
    Tone(float f, int samplerate, char *window);
    ~Tone();

    void reset();
    int snapshot();
    bool detectRisingEdge(int dF);
    bool detectPeak();

    float magnitude();
    void iteration(float s, int n);
    void detect(short *data);

    /* used by analyzer to manage buffering */
    int scnt;
    int sidx;
    float scale;

private:
    float freq;
    int avgsum;
    int avgnum;
    int avgval;
    float realW;
    float imagW;
    float d1;
    float d2;
    float *window;
};

class Analyzer {
public:
    Analyzer(int samplerate, int samplesize, int numchannels,
             char *window=NULL, char *tonemap=NULL);
    ~Analyzer();

    static Analyzer* create(int samplerate, int samplesize,
                            int numchannels, char *window,
                            char *tonemap);
    void soundinput(unsigned char *data, int size);
#ifdef DISPLAYASCII
    void print();
#endif
    void snapshot();
    static bool tonemap(const char *map, int *div=NULL,
                        int *start=NULL, int *count=NULL);

    float *spectrum;
    unsigned char *colors;
    int numtones;
    int samples;
    int tdiv;
private:
    Tone **tones;
#ifdef DISPLAYASCII
    void textcolor(int attr, int fg);
    void textcolor(unsigned char N);
#endif
    float scale;
    int samplerate;
    int samplesize;
    int numchannels;
    short *buffer;
    int buffer_size;
    int transform_idx;

    struct timespec lb;
    u_int64_t beat_duration;
    bool isBeat();
    void colorPeaks();
};

#endif //ANALYZER_H
