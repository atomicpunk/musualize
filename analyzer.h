#ifndef ANALYZER_H
#define ANALYZER_H

//#define TESTOUTPUT 1
#define OUTPUT_DELAY_MSEC 5
#define WAVELENGTHS 16

class Tone {
public:
    Tone(float f, int samplerate, char *window);
    ~Tone();

    float magnitude();
    void reset();
    void iteration(float s);
    int detect(short *data);
    void detectAverage(short *data);
    int returnAverage();

    /* used by analyzer to manage buffering */
    int scnt;
    int sidx;

private:
    int avgsum;
    int avgnum;
    int avgval;
    float freq;
    float realW;
    float imagW;
    float d1;
    float d2;
    float y;
    float window;
    float scale;
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
    void print();
    void snapshot();
    static bool tonemap(const char *map, int *div=NULL,
                        int *start=NULL, int *count=NULL);

    int *spectrum;
    int numtones;
    int samples;
private:
    Tone **tones;
    void textcolor(int attr, int fg);
    void textcolor(int N);
    int detectTone(short *data, int N, Tone *t);
    void detectTones(short *data, int N, int start, int end);
    int samplerate;
    int samplesize;
    int numchannels;
    short *buffer;
    int buffer_size;
    int transform_idx;
};

#endif //ANALYZER_H
