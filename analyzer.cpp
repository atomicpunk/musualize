#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "defines.h"
#include "analyzer.h"
#include <math.h>
#include <stdlib.h>

#ifdef DISPLAYASCII

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

#else

#define BLACK 		0
#define BLUE		1
#define GREEN		2
#define CYAN		3
#define RED		4
#define MAGENTA		5
#define YELLOW		6
#define	WHITE		7

#endif

Tone::Tone(float f, int samplerate, char *win) :
#ifdef GOERTZEL
    freq(f)
{
    float theta = 2.0 * M_PI * f / (float)samplerate;
    realW = 2.0*cos(theta);
    imagW = sin(theta);
    window = 0;
#if (DETECTION == 2)
    int ws = f*OUTPUT_DELAY_MSEC/1000;
    if(ws > WAVELENGTHS) ws = WAVELENGTHS;
    if(ws < 1)
    {
        scnt = (int)((float)samplerate / f);
        scale = scnt*FSCALE*WAVELENGTHS/3;
    }
    else if(ws < 2)
    {
        scnt = (int)((float)samplerate / f);
        scale = scnt*FSCALE*WAVELENGTHS/2;
    }
    else
    {
        scnt = (int)(ws * (float)samplerate / f);
        scale = scnt*FSCALE*WAVELENGTHS/ws;
    }
#else
    scnt = (int)(WAVELENGTHS * (float)samplerate / f);
    scale = scnt*FSCALE;
#endif
    sidx = scnt;
    avgsum = 0;
    avgnum = 0;
    avgval = 0;

    if((win)&&!strcmp(win, "hamming"))
    {
        window = 0.54 - (0.46 * cos(theta));
        scale /= 8;
    }
    else if((win)&&!strcmp(win, "blackman"))
    {
        window = 0.426591 - (.496561*cos(theta)) + (.076848*cos(2.0*theta));
        scale /= 60;
    }
#else
    Ft(f), Fs(samplerate)
{
    w = 2 * M_PI * Ft;
#endif
    for(int i = 0; i < TONE_HISTORY; i++)
    {
        history[i] = 0;
    }
    reset();
}

void Tone::reset()
{
#ifdef GOERTZEL
    d1 = 0;
    d2 = 0;
#else
    t = 0;
    x = 1;
    v = 0;
#endif
}

#ifdef GOERTZEL
void Tone::iteration(float s)
{
    //float ws = (window > 0)?(window*s):s;
    float y = s + (realW*d1) - d2;
    d2 = d1;
    d1 = y;
}

float Tone::magnitude()
{
    float real = (0.5 * realW * d1) - d2;
    float imag = imagW * d1;
    return sqrt((real*real) + (imag*imag))/scale;
}

void Tone::detect(short *data)
{
    reset();
    for(int i = 0; i < scnt; i++)
    {
        iteration((float)data[i]);
    }
    avgsum += (int)(magnitude());
    avgnum++;
}
#else
float Tone::angle(float t)
{
    float a = w * t;
    int i = (int)(a/(2.0*M_PI));
    float ra = a - ((float)i*2.0*M_PI);
    if(ra >= M_PI)
        return (2.0*M_PI) - ra;
    else
        return ra;
}

void Tone::iteration(int sample)
{
    // INPUTS t, x, v
    A = sqrt((x*x) + ((v*v)/(w*w)));
    p = acos(x/A) - angle(t);
    t += (1/Fs);
    x = A*cos(angle(t)+p);
    v = -1*DAMPING_FORCE*w*A*sin(angle(t)+p);
}

void Tone::print()
{
    iteration(0);
    printf("t=%.6f\tx=%.6f\tv=%.6f\tA=%.6f\tp=%.6f\n", t, x, v, A, p);
}
#endif

int Tone::snapshot()
{
#ifdef GOERTZEL
    if(avgnum > 0)
    {
        avgval = avgsum / avgnum;
        avgsum = 0;
        avgnum = 0;
    }
    memmove(&history[1], &history[0], (TONE_HISTORY-1)*sizeof(int));
    history[0] = avgval;
    return avgval;
#else
    iteration(0);
    return x;
#endif
}

Tone::~Tone()
{
}

Analyzer::Analyzer(int r, int t, int n, char *w, char *m) :
    samplerate(r), samplesize(t), numchannels(n)
{
    int i, j, idx1, idx2, tdiv, tnum;

    if(m)
      tonemap(m, &tdiv, &idx1, &idx2);
    else
      tonemap("1:0:-1", &tdiv, &idx1, &idx2);

    float f, df = pow(2.0, 1.0/(12.0*tdiv));
    tnum = tdiv*120;
    float *notes = new float[tnum];
    buffer_size = WAVELENGTHS*(r/1600)*100;
    printf("BUFFERSIZE = %d\n", buffer_size);
    buffer = new short[buffer_size];
    transform_idx = buffer_size;

    for(i = 0, f = 27.5; i < 10; i++, f*=2)
    {
        for(j = -9*tdiv; j < 3*tdiv; j++)
        {
            notes[(i*12*tdiv)+j+(9*tdiv)] = f*pow(df, j);
        }
    }

    numtones = idx2 - idx1;
    tones = new Tone*[numtones];
    spectrum = new int[numtones];
    colors = new unsigned char[numtones];
    for(i = idx1; i < idx2; i++)
    {
        tones[i-idx1] = new Tone(notes[i], r, w);
    }
    delete notes;
}

Analyzer::~Analyzer()
{
    int i;
    for(i = 0; i < numtones; i++)
    {
        delete tones[i];
    }
    delete tones;
    delete buffer;
    delete spectrum;
    delete colors;
}

Analyzer* Analyzer::create(int samplerate, int samplesize,
                   int numchannels, char *window, char *tonemap)
{
    if((samplesize != 2)||(numchannels != 1))
        return NULL;

    return new Analyzer(samplerate, samplesize, numchannels, window, tonemap);
}

bool Analyzer::tonemap(const char *tmap, int *div, int *start, int *count)
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
        free(map);
        return false;
    }
    t = v[0] * 120;
    if(v[1] >= t)
    {
        fprintf(stderr, "tonemap start is beyond the end of the array");
        free(map);
        return false;
    }

    if(v[2] == -1)
        v[2] = t;

    v[2] += v[1];
    if(v[2] > t)
    {
        fprintf(stderr, "tonemap count is beyond the end of the array");
        free(map);
        return false;
    }

    if((div)&&(start)&&(count))
    {
        *div = v[0];
        *start = v[1];
        *count = v[2];
    }
    free(map);
    return true;
}

#ifdef DISPLAYASCII

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

#endif

void Analyzer::soundinput(unsigned char *data, int size)
{
    int i, j, idx = 0, N=size/samplesize;

    if(N < buffer_size)
    {
        memmove(&buffer[0], &buffer[N], (buffer_size-N)*samplesize);
        idx = buffer_size - N;
    }

    for(i = 0, j = idx; (i < N)&&(j < buffer_size); i++, j++)
    {
        buffer[j] = data[(i*2)] | data[(i*2)+1] << 8;
    }

#ifdef GOERTZEL
    for(i = 0; i < numtones; i++)
    {
        tones[i]->sidx = (tones[i]->sidx - N < 0)?0:(tones[i]->sidx - N);
        while(buffer_size - tones[i]->sidx >= tones[i]->scnt)
        {
            tones[i]->detect(&buffer[tones[i]->sidx]);
            tones[i]->sidx += tones[i]->scnt;
        }
    }
#else // NOT GOERTZEL
/*
    for(i = 0; i < numtones; i++)
    {
        for(int j = idx; j < buffer_size; j++)
        {
            tones[i]->iteration(buffer[j]);
        }
    }
*/
#endif
}

#ifdef DISPLAYASCII
void Analyzer::print()
{
#ifdef GOERTZEL
    snapshot();
    for(int i = 0; i < numtones; i++)
    {
        textcolor(spectrum[i]);
        printf("%3d", spectrum[i]);
    }
    printf("\n");
    textcolor(RESET, WHITE);
#else
    tones[456]->print();
#endif
}
#endif

void Analyzer::snapshot()
{
    for(int i = 0; i < numtones; i++)
    {
        spectrum[i] = tones[i]->snapshot();
        if(detectRisingEdge(tones[i]->history))
            colors[i] = RED;
        else
            colors[i] = GREEN;
    }
}

bool Analyzer::detectRisingEdge(int *t)
{
    for(int i = 0; i < 1; i++)
    {
        if(t[i] - t[i+1] < 5)
            return false;
    }
    return true;
}
