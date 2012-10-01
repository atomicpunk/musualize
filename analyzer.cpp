#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "analyzer.h"
#include <math.h>
#include <stdlib.h>
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

char note[12][3] = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "G#", "A", "Bb", "B"};

Tone::Tone(float f, int samplerate, char *w) :
    freq(f)
{
    float theta = 2.0 * M_PI * f / (float)samplerate;
    realW = 2.0*cos(theta);
    imagW = sin(theta);
    window = 0;
    scale = samplerate;
    if((w)&&!strcmp(w, "hamming"))
    {
        window = 0.54 - (0.46 * cos(theta));
        scale /= 8;
    }
    else if((w)&&!strcmp(w, "blackman"))
    {
        window = 0.426591 - (.496561*cos(theta)) + (.076848*cos(2.0*theta));
        scale /= 60;
    }
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
    return sqrt((real*real) + (imag*imag))/scale;
}

void Tone::iteration(float s)
{
    float ws = (window > 0)?(window*s):s;
    y = ws + (realW*d1) - d2;
    d2 = d1;
    d1 = y;
}

Tone::~Tone()
{
}

Analyzer::Analyzer(int r, int t, int n, char *w, char *m) :
    samplerate(r), samplesize(t), numchannels(n)
{
    int i, j, idx1, idx2, tdiv, tnum;
    tonemap(m, &tdiv, &idx1, &idx2);
    double f, df = pow(2.0, 1.0/(12.0*tdiv));
    tnum = tdiv*120;
    double *notes = new double[tnum];
    transform_idx = BUFFER_SIZE;

    for(i = 0, f = 27.5; i < 10; i++, f*=2)
    {
        for(j = -9*tdiv; j < 3*tdiv; j++)
        {
            notes[(i*12*tdiv)+j+(9*tdiv)] = f*pow(df, j);
        }
    }

    numtones = idx2 - idx1;
    tones = new Tone*[numtones];
    for(i = idx1; i < idx2; i++)
    {
        tones[i-idx1] = new Tone(notes[i], r, w);
    }
    delete notes;
}

Analyzer::~Analyzer()
{

}

bool Analyzer::tonemap(char *tmap, int *div, int *start, int *count)
{
    if(tmap == NULL)
       return false;

    char *m, *map = strdup(tmap);
    int i, t, v[3]={0,0,0};

    for(i = 0, m = strtok(map, ":, \t"); (i < 3)&(m != NULL); 
        m = strtok(NULL, ":, \t"), i++)
    {
        v[i] = atoi(m);
    }
    if((v[0] < 1)||(v[0] > 8)||(v[1] < 0)||(v[2] < -1))
    {
        fprintf(stderr, "tonemap is <1:8>:<0:N-1>:<-1,1:N>");
        return false;
    }
    t = v[0] * 120;
    if(v[1] >= t)
    {
        fprintf(stderr, "tonemap start is beyond the end of the array");
        return false;
    }

    if(v[2] == -1)
        v[2] = t;

    v[2] += v[1];
    if(v[2] > t)
    {
        fprintf(stderr, "tonemap count is beyond the end of the array");
        return false;
    }

    if((div)&&(start)&&(count))
    {
        *div = v[0];
        *start = v[1];
        *count = v[2];
    }
    return true;
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
    return (int)(t->magnitude());
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
        for(i = 0; i < numtones; i++)
        {
            printf("%3s", note[i%12]);
        }
        printf("\n");
*/
        detectTones(&buffer[transform_idx], TRANSFORM_SIZE, 0, numtones);
        for(i = 0; i < numtones; i++)
        {
            idx = (int)(tones[i]->magnitude());
            textcolor(idx);
            printf("%3d", idx);
        }
        printf("\n");
        textcolor(RESET, WHITE);
        transform_idx += TRANSFORM_SIZE;
    }
#endif
}

Analyzer* Analyzer::analyzer_init(int samplerate, int samplesize,
                   int numchannels, char *window, char *tonemap)
{
    if((samplesize != 2)||(numchannels != 1))
        return NULL;

    return new Analyzer(samplerate, samplesize, numchannels, window, tonemap);
}
