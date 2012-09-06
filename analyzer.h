#ifndef ANALYZER_H
#define ANALYZER_H

void analyzer_init(int samplerate, int samplesize, int numchannels);
void analyzer_input(unsigned char *buffer, int size);

class Analyzer {
public:
    Analyzer(int samplerate, int samplesize, int numchannels);
    ~Analyzer();

    void soundinput(unsigned char *buffer, int size);
private:
    void left16(short *buffer, int length);
    int samplerate;
    int samplesize;
    int numchannels;
};

#endif //ANALYZER_H
