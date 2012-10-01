#include <stdio.h>
#include <string.h>
#include "analyzer.h"
#include <math.h>
#define RESET		0
#define BRIGHT 		1
#define DIM		2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7

Analyzer *analyzer = NULL;
char note[12][3] = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "G#", "A", "Bb", "B"};

Tone::Tone(float f, int samplerate) :
    freq(f)
{
    float theta = 2.0 * M_PI * f / (float)samplerate;
    realW = 2.0*cos(theta);
    imagW = sin(theta);
    reset();
}

void Tone::reset()
{
    d1 = 0;
    d2 = 0;
    y = 0;
}

float Tone::magnitude()
{
    float real = (0.5 * realW * d1) - d2;
    float imag = imagW * d1;
    return sqrt((real*real) + (imag*imag));
}

void Tone::iteration(float s)
{
    y = s + (realW*d1) - d2;
    d2 = d1;
    d1 = y;
}

Tone::~Tone()
{
}

Analyzer::Analyzer(int r, int t, int n) :
    samplerate(r), samplesize(t), numchannels(n)
{
    int i, j, oct = 10;
    double f, df = pow(2.0, 1.0/12.0);
    transform_idx = BUFFER_SIZE;
    numtones = oct*12;
    idx1 = 0;
    idx2 = 120;
    tones = new Tone*[numtones];

    for(i = 0, f = 27.5; i < oct; i++, f*=2)
    {
        for(j = -9; j < 3; j++)
        {
            tones[(i*12)+j+9] = new Tone(f*pow(df, j), r);
        }
    }
}

Analyzer::~Analyzer()
{

}

void Analyzer::textcolor(int attr, int fg)
{
    printf("%c[%d;%dm", 0x1B, attr, fg + 30);
}

void Analyzer::textcolor(int N)
{
    int attr, fg;

    if(N < 1)
    {
        attr = DIM;
        fg = BLACK;
    }
    else if(N < 20)
    {
        attr = BRIGHT;
        fg = BLUE;
    }
    else if(N < 40)
    {
        attr = BRIGHT;
        fg = MAGENTA;
    }
    else if(N < 60)
    {
        attr = BRIGHT;
        fg = CYAN;
    }
    else if(N < 80)
    {
        attr = BRIGHT;
        fg = GREEN;
    }
    else if(N < 100)
    {
        attr = BRIGHT;
        fg = YELLOW;
    }
    else
    {
        attr = BRIGHT;
        fg = RED;
    }
    printf("%c[%d;%dm", 0x1B, attr, fg + 30);
}

int Analyzer::detectTone(short *data, int N, Tone *t)
{
    t->reset();
    for(int i = 0; i < N; i++)
    {
        t->iteration((float)data[i]);
    }
    return (int)(t->magnitude()/samplerate);
}

void Analyzer::detectTones(short *data, int N, int start, int end)
{
    int i, j;

    for(int i = start; i < end; i++)
    {
        tones[i]->reset();
    }

    for(int i = 0; i < N; i++)
    {
        for(int j = start; j < end; j++)
        {
            tones[j]->iteration((float)data[i]);
        }
    }
}

void Analyzer::soundinput(unsigned char *data, int size)
{
#if 0
    int i;
    int N = size/samplesize;
    static int j = 0;
    short s;

    for(i = 0; i < N; i++, j++)
    {
        s = data[(i*2)] | data[(i*2)+1] << 8;
        printf("%04X ", (unsigned short)s);
        if(j%42 == 0)
            printf("\n");
    }
#else
    int i, idx = 0, N=size/samplesize;

    if(N < BUFFER_SIZE)
    {
        memmove(&buffer[0], &buffer[N], (BUFFER_SIZE-N)*samplesize);
        idx = BUFFER_SIZE - N;
    }

    for(i = 0; (i < N)&&(idx < BUFFER_SIZE); i++, idx++)
    {
        buffer[idx] = data[(i*2)] | data[(i*2)+1] << 8;
    }

    transform_idx = (transform_idx - N < 0)?0:(transform_idx - N);
    if(BUFFER_SIZE - transform_idx >= TRANSFORM_SIZE)
    {
/*
        textcolor(RESET, WHITE);
        for(i = idx1; i < idx2; i++)
        {
            printf("%3s", note[i%12]);
        }
        printf("\n");
*/
        detectTones(&buffer[transform_idx], TRANSFORM_SIZE, idx1, idx2);
        for(i = idx1; i < idx2; i++)
        {
            idx = (int)(tones[i]->magnitude()/samplerate);
            textcolor(idx);
            printf("%3d", idx);
        }
        printf("\n");
        textcolor(RESET, WHITE);
        transform_idx += TRANSFORM_SIZE;
    }
#endif
}

void analyzer_input(unsigned char *buffer, int size)
{
    if(analyzer)
        analyzer->soundinput(buffer, size);
}

void analyzer_init(int samplerate, int samplesize, int numchannels)
{
    if((samplesize != 2)||(numchannels != 1))
        return;

    if(analyzer)
        delete analyzer;

    analyzer = new Analyzer(samplerate, samplesize, numchannels);
}
