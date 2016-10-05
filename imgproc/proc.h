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
#endif // PROC_H_INCLUDED
