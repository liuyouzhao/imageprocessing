#ifndef PROC_H_INCLUDED
#define PROC_H_INCLUDED
#include <vector>


#ifndef PROC_UTILS_H_INCLUDED
#define PROC_UTILS_H_INCLUDED

#define COLOR_RGBA 1

#define LETS_GO_TEST 0


#define SAMPLE_NUMBER 2000
#define SAMPLE_MERGED_WIDTH 50
#define SAMPLE_MERGED_HEIGHT 40
#define TRAINING_TIMES 50
#define SAMPLE1_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/res/faces"
#define SAMPLE2_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/res/nonfaces"

#define FEATURE_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/f_v"
#define CLASSFIER_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/clssf"
#define WEAK_CLASSFIER_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/weak_clssf"

#define STRONG_CLASSFIER_FILE "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/strong_clssf/strong_clssf"
#define STRONG_CLASSFIER_FILE1 "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/strong_clssf/strong_clssf01"
#define STRONG_CLASSFIER_FILE2 "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/strong_clssf/strong_clssf02"

#define CLSSF_INDEX_FILE "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/clssf_index"

#define WEIGHT_LOSS_PATH "/home/hujia/workspace/imgproc/imageprocessing/imgproc/train/weights"

#define TEST_FILE "/home/hujia/workspace/imgproc/imageprocessing/imgproc/res/tests/test.jpg"
//#define TEST_FILE "/home/hujia/workspace/imgproc/imageprocessing/imgproc/res/tests/22.bmp"

#define SAMPLE_BLOCK_WIDTH 20

#define THREHOLD_ACCURATE 40

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

#define COLOR_RGB 1

#endif // PROC_UTILS_H_INCLUDED


struct BinaryProc {
    unsigned char* (*process)(u8* p, u8* out, u32 w, u32 h, u8 t);
};

struct GreyProc {
    unsigned char* (*process)(u8* p, u8* out, u32 w, u32 h, int type);
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
    int (*haarlike_linear_horizon)(u32 *integ, u32 w, u32 h, u32 sw, u32 sh, u8 x, u8 y, u8 bw, u8 bh, int **ftout);
    int (*haarlike_linear_vert)(u32 *integ, u32 w, u32 h, u32 sw, u32 sh, u8 x, u8 y, u8 bw, u8 bh, int **ftout);
    int (*haarlike_point)(u32 *integ, u32 w, u32 h, u32 sw, u32 sh, u8 x, u8 y, u8 bw, u8 bh, int **ftout);

};


struct __feature_value
{
    int sample_id;
    int index;
    int value;
    int up_down;
};


struct __clssf {
    u8 x;
    u8 y;
    u8 bw;
    u8 bh;
    u8 tt;
    u8 tn; /// threhold direction: 0+, 1-
    u32 ss;
    int thrhd;
    double em;
    double am;
};

class __strong_clssf {
public:
    int nwk;
    std::vector<struct __clssf*> vwkcs;
};

struct __possi_rect {
    int x;
    int y;
    int w;
    int h;
};

void sort(int *a, int left, int right);
void sort_fv_by_val(struct __feature_value **fv, int left, int right);

int read_file(const char* file, char* data, int len);
int write_file(const char* file, char* data, int len);
int write_file_by_id(const char* header, int x, int y, int bw, int bh, int tt, int tn, char* data, int len);
#endif // PROC_H_INCLUDED
