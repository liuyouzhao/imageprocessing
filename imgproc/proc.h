#ifndef PROC_H_INCLUDED
#define PROC_H_INCLUDED

#ifndef PROC_UTILS_H_INCLUDED
#define PROC_UTILS_H_INCLUDED

#define COLOR_RGBA 1

#define SAMPLE_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/res/faces"
#define FEATURE_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/f_v"
#define CLASSFIER_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/clssf"
#define WEAK_CLASSFIER_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/weak_clssf"
#define STRONG_CLASSFIER_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/strong_clssf"

#define WEIGHT_FILE "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/s_w"

#define SAMPLE_BLOCK_WIDTH 20
#define SAMPLE_NUMBER 2000
#define SAMPLE_MERGED_WIDTH 50
#define SAMPLE_MERGED_HEIGHT 40

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
    int (*haarlike_integral)(u8 *p, u32 *out, u32 w, u32 h);
    int (*haarlike_edge_horizon)(u32 *integ, u32 w, u32 h, u32 sw, u32 sh, u8 x, u8 y, u8 bw, u8 bh, int **ftout);
    int (*haarlike_edge_vert)(u32 *integ, u32 w, u32 h, u32 sw, u32 sh, u8 x, u8 y, u8 bw, u8 bh, int **ftout);
};

/*
* Inner structs
*/
struct __clssf {
    u8 x;
    u8 y;
    u8 bw;
    u8 bh;
    u8 tt;
    u8 tn;
    double em;
    double am;
};

#endif // PROC_H_INCLUDED
