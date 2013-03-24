/*
 * Copyright (c) 2013, Todd E Brandt <tebrandt@frontier.com>.
 *
 * This program is licensed under the terms and conditions of the
 * GPL License, version 2.0.  The full text of the GPL License is at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifndef DEFINES_H
#define DEFINES_H

// display defines
#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920
#define WINDOW_X 0
#define WINDOW_Y 0

// analyzer defines
//#define DISPLAYASCII
#ifndef DISPLAYASCII
#define TONE_HISTORY 200
#endif

#define DETECTION 2 // 0 = raw, 1 = avg, 2 = avg+raw
#define LOWFREQ 27.5
#define SCALEINC 1
#define DISPLAYRATE 400
#define OUTPUT_DELAY_MSEC 50
#define WAVELENGTHS 32
#define LINESIZE ((int)(DISPLAYRATE*OUTPUT_DELAY_MSEC/1000))
#if (OUTPUT_DELAY_MSEC*DISPLAYRATE > 10000)
#undef LINESIZE
#define LINESIZE 1
#endif

#ifdef DISPLAYASCII

#define RESET           0
#define BRIGHT          1
#define DIM             2
#define UNDERLINE       3
#define BLINK           4
#define REVERSE         7
#define HIDDEN          8

#define BLACK           0
#define RED             1
#define GREEN           2
#define YELLOW          3
#define BLUE            4
#define MAGENTA         5
#define CYAN            6
#define WHITE           7

#else

#define BLACK           0
#define BLUE            1
#define GREEN           2
#define CYAN            3
#define RED             4
#define MAGENTA         5
#define YELLOW          6
#define WHITE           7

#endif

extern bool paused;
extern unsigned char primary_color;
extern unsigned char highlight_color;
extern int verbose;

#endif //DEFINES_H
