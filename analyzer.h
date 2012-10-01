#ifndef ANALYZER_H
#define ANALYZER_H

#define BUFFER_SIZE 1500
#define TRANSFORM_SIZE 1000

class Tone {
public:
    Tone(float f, int samplerate, char *window);
    ~Tone();
    float freq;
    float realW;
    float imagW;
    float d1;
    float d2;
    float y;
    float window;
    float scale;
    float magnitude();
    void reset();
    void iteration(float s);
};

class Analyzer {
public:
    Analyzer(int samplerate, int samplesize, int numchannels, 
             char *window=NULL, char *tonemap=NULL);
    ~Analyzer();

    static Analyzer* analyzer_init(int samplerate, int samplesize, int numchannels, 
                                   char *window, char *tonemap);
    void soundinput(unsigned char *data, int size);
    static bool tonemap(char *map, int *div=NULL, int *start=NULL, int *count=NULL);
private:
    Tone **tones;
    int numtones;
    void textcolor(int attr, int fg);
    void textcolor(int N);
    int detectTone(short *data, int N, Tone *t);
    void detectTones(short *data, int N, int start, int end);
    int samplerate;
    int samplesize;
    int numchannels;
    short buffer[BUFFER_SIZE];
    int transform_idx;
};

#endif //ANALYZER_H
