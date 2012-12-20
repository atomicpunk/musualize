#ifndef DEFINES_H
#define DEFINES_H

#define TONE_HISTORY 5
#define SMAX 30

// which algorithm?
#define GOERTZEL

#ifdef GOERTZEL // goertzel

#define DETECTION    2 // 0 = raw, 1 = avg, 2 = avg+raw
#define FSCALE 10
#define DISPLAYRATE 400
#define OUTPUT_DELAY_MSEC 14
#define WAVELENGTHS 128
#define LINESIZE ((int)(DISPLAYRATE*OUTPUT_DELAY_MSEC/1000))

#else // basilar membrane

#define DISPLAYASCII
#define DAMPING_FORCE 1

#endif

#endif //DEFINES_H
