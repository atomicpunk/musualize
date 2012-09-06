#include <stdio.h>
#include "analyzer.h"

void soundinput(void *buffer, int bytesize)
{

}

void analyzer_init(int samplerate, int sampletype, int numchannels)
{
    printf("%d, %d, %d\n", samplerate, sampletype, numchannels);
}
