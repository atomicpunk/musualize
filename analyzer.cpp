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

void textcolor(int attr, int fg)
{
    char command[13];

    /* Command is the control command to the terminal */
//    sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
    sprintf(command, "%c[%d;%dm", 0x1B, attr, fg + 30);
    printf("%s", command);
}

void textcolor(int N)
{
    int attr, fg;

    if(N < 1)
    {
        attr = HIDDEN;
        fg = WHITE;
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

Analyzer *analyzer = NULL;

float notes[] =
{/*  C     C#      D     Eb      E      F     F#      G     G#      A     Bb      B    */
   16.35, 17.32, 18.35,	19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87, // OCT0
   32.70, 34.65, 36.71,	38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27, 61.74, // OCT1
   65.41, 69.30, 73.42,	77.78, 82.41, 87.31, 92.50, 98.00, 103.8, 110.0, 116.5, 123.5, // OCT2
   130.8, 138.6, 146.8,	155.6, 164.8, 174.6, 185.0, 196.0, 207.7, 220.0, 233.1, 246.9, // OCT3
   261.6, 277.2, 293.7,	311.1, 329.6, 349.2, 370.0, 392.0, 415.3, 440.0, 466.2, 493.9, // OCT4
   523.3, 554.4, 587.3,	622.3, 659.3, 698.5, 740.0, 784.0, 830.6, 880.0, 932.3, 987.8, // OCT5
    1047,  1109,  1175,	 1245.0, 1319.0,  1397,  1480,  1568,  1661,  1760,  1865,  1976, // OCT6
    2093,  2217,  2349,	 2489,  2637,  2794,  2960,  3136,  3322,  3520,  3729,  3951, // OCT7
    4186,  4435,  4699,	 4978,  5274,  5588,  5920,  6272,  6645,  7040,  7459,  7902  // OCT8
};

char note[12][3] = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "G#", "A", "Bb", "B"};

Analyzer::Analyzer(int r, int t, int n) :
    samplerate(r), samplesize(t), numchannels(n)
{
    transform_idx = BUFFER_SIZE;
}

Analyzer::~Analyzer()
{

}

int Analyzer::detectTone(short *data, int N, float f)
{
    int i;

    float theta = 2.0 * M_PI * f / (float)samplerate;
    float realW = 2.0*cos(theta);
    float imagW = sin(theta);

    float d1 = 0.0;
    float d2 = 0.0;
    float y;

    float real;
    float imag;
    float mag;

    for(i = 0; i < N; i++)
    {
        y = (float)data[i] + (realW*d1) - d2;
        d2 = d1;
        d1 = y;
    }

    real = (0.5 * realW * d1) - d2;
    imag = imagW * d1;
    mag = sqrt((real*real) + (imag*imag));
    return (int)(mag/samplerate);
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
        for(i = 10; i < 81; i++)
        {
            printf("%3s", note[i%12]);
        }
        printf("\n");
*/
        for(i = 10; i < 81; i++)
        {
            N = detectTone(&buffer[transform_idx], TRANSFORM_SIZE, notes[i]);
            textcolor(N);
            printf("%3d", N);
        }
        printf("\n");
        transform_idx += TRANSFORM_SIZE;
        textcolor(RESET, WHITE);
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
