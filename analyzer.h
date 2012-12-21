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
    int history[TONE_HISTORY];

#ifdef GOERTZEL
    float magnitude();
    void iteration(float s, int n);
    void detect(short *data);

    /* used by analyzer to manage buffering */
    int scnt;
    int sidx;
    float scale;
#else
    void print();
    void iteration(int sample);
    float angle(float t);
#endif

private:
#ifdef GOERTZEL
    float freq;
    int avgsum;
    int avgnum;
    int avgval;
    float realW;
    float imagW;
    float d1;
    float d2;
    float *window;
#else
    float Fs; // sample frequency
    float Ft; // target frequency
    float b;  // damping force
    float w;  // angular frequency
    float A;
    float p;
    float t;
    float x;
    float v;
#endif
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
private:
    Tone **tones;
#ifdef DISPLAYASCII
    void textcolor(int attr, int fg);
    void textcolor(float N);
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
    bool detectRisingEdge(int *t);
};

#endif //ANALYZER_H
