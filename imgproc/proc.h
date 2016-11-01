#ifndef PROC_H_INCLUDED
#define PROC_H_INCLUDED
#include <vector>


#ifndef PROC_UTILS_H_INCLUDED
#define PROC_UTILS_H_INCLUDED

#define COLOR_RGBA 1

#define LETS_GO_TEST 1
#define PROCESS_SHOW 0


#define SAMPLE_NUMBER 2000
#define SAMPLE_MERGED_WIDTH 50
#define SAMPLE_MERGED_HEIGHT 40
#define TRAINING_TIMES 200
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
//#define TEST_FILE "/home/hujia/workspace/imgproc/imageprocessing/imgproc/res/tests/test4.jpg"



#define SAMPLE_BLOCK_WIDTH 20

#define THREHOLD_ACCURATE 40

#define T_ARR {1, 2, 1, 3, 2}
#define S_ARR {2, 1, 3, 1, 2}
#define TT_ARR {0, 1, 2, 3, 4}

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


    int fv_len;
    int* fvs;
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
    int rt;
};


#define PARIMITS_LAYERS_NUMBER 5
#define FEATURE_THREHOLD 1
#define HFV_NUM 64

#define HESSIAN_RADIO 6
#define HESSIAN_HAAR_SIZE 4
#define HESSIAN_ANGLE 60.0
#define HESSIAN_ANGLE_JUMP 30
#define HESSIAN_DESCRIPT_BLOCK 20
#define HESSIAN_DESCRIPT_NUM_BLOCK 4
#define HESSIAN_DESCRIPT_HAAR_SIZE 2

#define PI 3.1415926535897932

struct __hessian_value
{
    double* values;
    int level;
    int group;
};

struct __hessian_fv
{
    int x;
    int y;
    int hfv;
    int level;
    int group;
    int dir;
    int fvs[HFV_NUM];
    double s;
};


class SurfObject
{

public:
    SurfObject();
    ~SurfObject();

    void updateImage(u8* image, int w, int h);
    void updateCurrentIntegral();
    void updateAllParimitsFeatures();
    void updateCalculateExtremums();
    void updateCalculateDirections();
    void updateFeatureDescription();

    u8* getOriginalImage() {    return m_orignal_image; }
    int getWidth()  {   return m_width; }
    int getHeight() {   return m_height; }
    std::vector<struct __hessian_fv*> getFeatureValues() {    return m_feature_values; }


private:
    std::vector<struct __hessian_value*> m_parimit;
    std::vector<struct __hessian_fv*> m_feature_values;

    u8* m_orignal_image;

    u32* m_integral;

    int m_width;
    int m_height;
};

void sort(int *a, int left, int right);
void sort_fv_by_val(struct __feature_value **fv, int left, int right);
void sort_fv_by_val(struct __possi_rect **rects, int left, int right);

int read_file(const char* file, char* data, int len);
int write_file(const char* file, char* data, int len);
int write_file_by_id(const char* header, int x, int y, int bw, int bh, int tt, int tn, char* data, int len);

void show_image(u8* buffer, int w, int h);
void show_image_c(u8* buffer, int w, int h, int c);
void __perf_begin_time();
void __perf_end_time();

void load_color_image_from_file(const char* file, u8** image, int *w, int *h);
#endif // PROC_H_INCLUDED
