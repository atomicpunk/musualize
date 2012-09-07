#ifndef ANALYZER_H
#define ANALYZER_H

void analyzer_init(int samplerate, int samplesize, int numchannels);
void analyzer_input(unsigned char *buffer, int size);

#define BUFFER_SIZE 2000
#define TRANSFORM_SIZE 1000

class Analyzer {
public:
    Analyzer(int samplerate, int samplesize, int numchannels);
    ~Analyzer();

    void soundinput(unsigned char *data, int size);
private:
    int detectTone(short *data, int N, float f);
    int samplerate;
    int samplesize;
    int numchannels;
    short buffer[BUFFER_SIZE];
    int transform_idx;
};

#endif //ANALYZER_H
