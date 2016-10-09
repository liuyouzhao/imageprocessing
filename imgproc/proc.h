#ifndef PROC_H_INCLUDED
#define PROC_H_INCLUDED

#ifndef PROC_UTILS_H_INCLUDED
#define PROC_UTILS_H_INCLUDED

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

#define COLOR_RGB 1

#endif // PROC_UTILS_H_INCLUDED


struct BinaryProc {
    unsigned char* (*process)(u8* p, u8* out, u32 w, u32 h, u8 t);
};

struct GreyProc {
    unsigned char* (*process)(u8* p, u8* out, u32 w, u32 h);
};

struct LoneedgeProc {
    unsigned char* (*process)(u8* p, u8* out, u32 w, u32 h, u8 t);
};

struct GradientProc {
    unsigned char* (*process)(u8* p, u8* out, u32 w, u32 h);
};

struct ConvoluteProc {
    unsigned char* (*process)(u8 *p, u8 *out, u32 w, u32 h, u8 *opera, u32 ow, float fix);
};

struct GaussProc {
    unsigned char* (*process)(u8 *p, u8 *out, u32 w, u32 h, int ow);
};

struct HaarlikeProc {
    int (*process)(u8* p, u32 w, u32 h,
                                int **ftout1, int **ftout2,
                                u32 *wf, u32 *hf, u32 b, int type);
};

#endif // PROC_H_INCLUDED
