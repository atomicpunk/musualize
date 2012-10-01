#ifndef ANALYZER_H
#define ANALYZER_H

void analyzer_init(int samplerate, int samplesize, int numchannels);
void analyzer_input(unsigned char *buffer, int size);

#define BUFFER_SIZE 1500
#define TRANSFORM_SIZE 1000

class Tone {
public:
    Tone(float f, int samplerate);
    ~Tone();
    float freq;
    float realW;
    float imagW;
    float d1;
    float d2;
    float y;
    float magnitude();
    void reset();
    void iteration(float s);
};

class Analyzer {
public:
    Analyzer(int samplerate, int samplesize, int numchannels);
    ~Analyzer();

    void soundinput(unsigned char *data, int size);
private:
    Tone **tones;
    int numtones;
    int idx1;
    int idx2;
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
