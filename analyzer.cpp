#include <stdio.h>
#include "analyzer.h"

Analyzer *analyzer = NULL;

Analyzer::Analyzer(int r, int t, int n) :
    samplerate(r), samplesize(t), numchannels(n)
{

}

Analyzer::~Analyzer()
{

}

void Analyzer::soundinput(unsigned char *buffer, int size)
{
    int i;
    static int j = 0;
    unsigned short s;
    for(i = 0; i < size; i+=2, j++)
    {
        s = buffer[i] | buffer[i+1] << 8;
        printf("%04X ", s);
        if(j%16 == 0)
           printf("\n");
    }
}

void analyzer_input(unsigned char *buffer, int size)
{
    if(analyzer)
        analyzer->soundinput(buffer, size);
}

void analyzer_init(int samplerate, int samplesize, int numchannels)
{
    if(analyzer)
        delete analyzer;

    analyzer = new Analyzer(samplerate, samplesize, numchannels);
}
