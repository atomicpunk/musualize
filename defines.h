#ifndef DEFINES_H
#define DEFINES_H

#define TONE_HISTORY 5

// which algorithm?
#define GOERTZEL

#ifdef GOERTZEL // goertzel

#define DETECTION 2 // 0 = raw, 1 = avg, 2 = avg+raw
#define LOWFREQ 27.5
#define SCALEINC 1
#define DISPLAYRATE 400
#define OUTPUT_DELAY_MSEC 50
#define WAVELENGTHS 32
//#define LINESIZE ((int)(DISPLAYRATE*OUTPUT_DELAY_MSEC/1000))
#define LINESIZE 10

#else // basilar membrane

#define DISPLAYASCII
#define DAMPING_FORCE 1

#endif

#endif //DEFINES_H
