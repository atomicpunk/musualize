#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "defines.h"
#include "analyzer.h"
#include <math.h>
#include <stdlib.h>

unsigned char primary_color = GREEN;
unsigned char highlight_color = RED;

Tone::Tone(float f, int samplerate, char *win) :
#ifdef GOERTZEL
    freq(f)
{
    float theta = 2.0 * M_PI * f / (float)samplerate;
    realW = 2.0*cos(theta);
    imagW = sin(theta);
#if (DETECTION == 2)
    int ws = f*OUTPUT_DELAY_MSEC/1000;
    if(ws > WAVELENGTHS) ws = WAVELENGTHS;
    if(ws < 2)
    {
        scnt = (int)((float)samplerate / f);
    }
    else
    {
        scnt = (int)(ws * (float)samplerate / f);
    }
#elif (DETECTION == 1)
    scnt = (int)(WAVELENGTHS * (float)samplerate / f);
#else
    scnt = WAVELENGTHS*(samplerate/1600)*100;
#endif
    scale = scnt;
    sidx = scnt;
    avgsum = 0;
    avgnum = 0;
    avgval = 0;
    window = NULL;

    if((win)&&!strcmp(win, "hamming"))
    {
        window = new float[scnt];
        for(int i = 0; i < scnt; i++)
        {
            theta = 2.0 * M_PI * ((float)i / (float)scnt);
            window[i] = 0.54 - (0.46 * cos(theta));
        }
    }
    else if((win)&&!strcmp(win, "blackman"))
    {
        window = new float[scnt];
        for(int i = 0; i < scnt; i++)
        {
            theta = 2.0 * M_PI * ((float)i / (float)scnt);
            window[i] = 0.426591 - (.496561*cos(theta)) + (.076848*cos(2.0*theta));
        }
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

Tone::~Tone()
{
    if(window)
        delete window;
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
void Tone::iteration(float s, int n)
{
    float y = (realW*d1) - d2;
    if(window)
        y += window[n]*s;
    else
        y += s;
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
        iteration((float)data[i], i);
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

bool Tone::detectRisingEdge(int dF)
{
    for(int i = 0; i < TONE_HISTORY-1; i++)
         if(history[i] - history[i+1] < 3)
            return false;
    return true;
}

bool Tone::detectPeak()
{
    int i;
    if(history[1] - history[0] > 0)
    {
        for(i = 1; i < TONE_HISTORY-1; i++)
            if(history[i] <= history[i+1])
                return false;
        return true;
    }
    return false;
}

Analyzer::Analyzer(int r, int t, int n, char *w, char *m) :
    samplerate(r), samplesize(t), numchannels(n)
{
    int i, j, idx1, idx2, tnum;
    samples = 0;

    if(m)
      tonemap(m, &tdiv, &idx1, &idx2);
    else
      tonemap("1:0:-1", &tdiv, &idx1, &idx2);

    float f, df = pow(2.0, 1.0/(12.0*tdiv));
    tnum = tdiv*120;
    float *notes = new float[tnum];
    scale = 1;
    buffer_size = WAVELENGTHS*samplerate/LOWFREQ;
    buffer = new short[buffer_size];
    transform_idx = buffer_size;

    for(i = 0, f = LOWFREQ; i < 10; i++, f*=2)
    {
        for(j = -9*tdiv; j < 3*tdiv; j++)
        {
            notes[(i*12*tdiv)+j+(9*tdiv)] = f*pow(df, j);
        }
    }

    clock_gettime(CLOCK_REALTIME, &lb);
    beat_duration = (60000000)/128;
    numtones = idx2 - idx1;
    tones = new Tone*[numtones];
    spectrum = new float[numtones];
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

void Analyzer::textcolor(float N)
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
#if (DETECTION == 0)
        tones[i]->sidx = (tones[i]->sidx - N < 0)?0:(tones[i]->sidx - N);
#else
        tones[i]->sidx = (tones[i]->sidx - N < 0)?0:(tones[i]->sidx - N);
        while(buffer_size - tones[i]->sidx >= tones[i]->scnt)
        {
            tones[i]->detect(&buffer[tones[i]->sidx]);
            tones[i]->sidx += tones[i]->scnt;
        }
#endif
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
    samples += N;
}

#ifdef DISPLAYASCII
void Analyzer::print()
{
#ifdef GOERTZEL
    snapshot();
    for(int i = 0; i < numtones; i++)
    {
        textcolor(spectrum[i]);
        printf("%1.1f", spectrum[i]);
    }
    printf("\n");
    textcolor(RESET, WHITE);
#else
    tones[456]->print();
#endif
}
#endif

bool Analyzer::isBeat() {

    struct timespec tm;
    clock_gettime(CLOCK_REALTIME, &tm);
    u_int64_t t1 = (tm.tv_sec*1000000) + (tm.tv_nsec/1000);
    u_int64_t t2 = (lb.tv_sec*1000000) + (lb.tv_nsec/1000);
    if(t1 - t2 >= beat_duration)
    {
        lb = tm;
        return true;
    }
    return false;
}

void Analyzer::colorPeaks()
{
    float *s = spectrum, peak = 0;
    int i;
    for(i = 0; i < numtones; i++)
        if(s[i] > peak) peak = s[i];

    peak *= 0.9;
    for(i = 0; i < numtones; i++)
        if(s[i] > peak)
            colors[i] = highlight_color;
}

void Analyzer::snapshot()
{
    for(int i = 0; i < numtones; i++)
    {
#if (DETECTION == 0)
        tones[i]->scnt = buffer_size - tones[i]->sidx;
        tones[i]->scale = buffer_size - tones[i]->sidx;
        tones[i]->detect(&buffer[tones[i]->sidx]);
        tones[i]->sidx = buffer_size;
#endif
        spectrum[i] = (float)(tones[i]->snapshot())/scale;
        colors[i] = primary_color;

//        if(tones[i]->detectRisingEdge(scale))
//            colors[i] = highlight_color;

        if(spectrum[i] > 1)
        {
            scale += SCALEINC;
            spectrum[i] = 1;
        }
    }
    colorPeaks();

//    if(isBeat())
//        colors[0] = 23;
    samples = 0;
}
