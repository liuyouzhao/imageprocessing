#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "proc.h"
#include "proc_utils.h"

#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

static int statistic_clssfier(int x, int y, int bw, int bh, int tt, int tn, int *ftout, int siz);
static int get_next_weak_clss(int sw, int sh, int s, int t, int tt, int tn, int siz, int rt);

extern HaarlikeProc g_haarlike;
static double *g_weight = 0;

static int read_file(const char* file, char* data, int len) {
    FILE* fd = fopen(file, "r");
    int siz = fread((char*)data, len, 1, fd);

    if(siz != 1) {
        printf("Read File ERROR %s %d\n", file, siz);
        fclose(fd);
        return -1;
    }

    fclose(fd);

    //printf("%s\n", file);
    return 0;
}

static int write_file(const char* file, char* data, int len) {
    FILE* fd = fopen(file, "w");
    int nblock = fwrite((char*)data, len, 1, fd);

    if(nblock != 1) {
        printf("Error, write file error: %s   size: %d \n ", file, nblock);
        return -1;
    }

    fflush(fd);
    fclose(fd);

    //printf("%s\n", file);
}


int adaboost_merge_samples(const char* path, int file_num, int nw, int nh, int sw, int sh, u8 **merge_map) {

    int w = sw * nw;
    int h = sh * nh;

    *merge_map = (u8*) malloc(w * h);

    IplImage* p_frame = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);

    for(int y = 0; y < nh; y ++) {

        for(int x = 0; x < nw; x ++) {

            char file[256] = {0};
            sprintf(file, "%s/%d.bmp", path, y * nw + x + 1);

            cv::Mat src;
            src = cv::imread(file);

            if(!src.data)
            {
                printf("Error! file is empty! \n");
                return -1;
            }

            int nr = src.rows;
            int nc = src.cols;
            int c = src.channels();

            for(int i = 0; i < nr; i ++) {
                uchar* data = src.ptr<uchar>(i);
                for(int j = 0; j < nc; j ++) {
                    (*merge_map)[(y * sh + i) * w + (x * sw) + j] = data[j * c];
                }
            }
        }
    }
    memcpy(p_frame->imageData, *merge_map, p_frame->width * p_frame->height * p_frame->nChannels);

#if 0
    cvNamedWindow("show", 1);
    while(1) {
        cvShowImage("show", p_frame);
        char k = cvWaitKey(33);
    }
#endif
    return 0;
}


int adaboost_train(u8* sample, int w, int h, int sw, int sh)
{
    int s = 1;
    int t = 2;

    int *ftout1 = 0;
    int *ftout2 = 0;
    u32 wf = 0;
    u32 hf = 0;
    u32 *integ = 0;
    int bw, bh;

    int nw = w / sw;
    int nh = h / sh;

    g_weight = (double*) malloc(nw * nh * sizeof(double));

    double init_weight = 1.0f / 2000.0f;
    for(int i = 0; i < nw * nh; i ++) {
        g_weight[i] = init_weight;
    }
    write_file(WEIGHT_FILE, (char*)g_weight, nw * nh * sizeof(double));

    integ = (u32*) malloc(w * h * sizeof(u32));

    int ret = g_haarlike.haarlike_integral(sample, integ, w, h);

    /*
     * Edge 0 & 1
    */
    s = 4;
    t = 2;
    int *ftout = 0;
    for(int i = s ; i <= sh; i += s ) {

        for(int j = t; j <= sw; j += t ) {


            for(int y = 0; y < sh - i + 1; y ++ ) {
                for(int x = 0; x < sw - j + 1; x ++) {

                    int siz = g_haarlike.haarlike_edge_horizon(integ, w, h, sw, sh, x, y, j, i, &ftout);

#if 0
                    for(int n = 0; n < siz; n ++) {
                        printf("%d ", ftout[n]);
                    }
                    printf("============================================\n");
#endif

                    char file[256] = {0};
                    sprintf(file, "%s/f_v_%d_%d_%d_%d_%d_%d", FEATURE_PATH, x, y, j, i, 0, 0);
                    write_file(file, (char*)ftout, siz * sizeof(int));

                    //statistic_clssfier(x, y, j, i, 0, 0, ftout, siz);

                    free(ftout);
                }
            }
        }
    }


    for(int rt = 0; rt < 200; rt ++) {
        get_next_weak_clss(sw, sh, s, t, 0, 0, nw * nh, rt);
    }

    return 0;
}


static int statistic_clssfier(int x, int y, int bw, int bh, int tt, int tn, int *ftout, int siz) {

    struct __clssf cls = {
        .x = x,
        .y = y,
        .bw = bw,
        .bh = bh,
        .tt = tt,
        .tn = tn,
        .em = 0.0f,
        .am = 0.0f
    };

    double thehold = 0;
    double I = 1.0f;
    double em = 0.0f;
    double am = 0.0f;

    read_file(WEIGHT_FILE, (char*)g_weight, siz * sizeof(double));

    int zero = 0;
    for(int i = 0; i < siz; i ++) {
        thehold += (double)(g_weight[i] * (double)(ftout[i]));
        if(ftout[i] == 0) {
            zero ++;
        }
    }

    for(int i = 0; i < siz; i ++) {
        ftout[i] = ftout[i] >= thehold ? 1 : -1;
    }

    for(int i = 0; i < siz; i ++) {
        if(ftout[i] == -1)
            em += g_weight[i] * I;
    }

    double en = (1.0f - em) / em;
    double es = log(en);
    am = 0.5 * es;

    if(em == 0.0f) {
        am = 0.0f;
    }
    cls.em = em;
    cls.am = am;


    char file[256] = {0};
    sprintf(file, "%s/clssf_%d_%d_%d_%d_%d_%d", CLASSFIER_PATH, x, y, bw, bh, tt, tn);
    printf(" em[%f] am[%f] \n", em, am);
    write_file(file, (char*)(&cls), sizeof(struct __clssf));

    return 0;
}

static int get_next_weak_clss(int sw, int sh, int s, int t, int tt, int tn, int siz, int rt)
{

    int *ftout = (int*) malloc(siz * sizeof(int));
    for(int i = s ; i <= sh; i += s )
    {
        for(int j = t; j <= sw; j += t )
        {

            for(int y = 0; y < sh - i + 1; y ++ )
            {
                for(int x = 0; x < sw - j + 1; x ++)
                {
                    char file[256] = {0};
                    sprintf(file, "%s/f_v_%d_%d_%d_%d_%d_%d", FEATURE_PATH, x, y, j, i, 0, 0);

                    memset(ftout, 0, siz * sizeof(int));

                    read_file(file, (char*)ftout, siz * sizeof(int));

                    statistic_clssfier(x, y, j, i, tt, tn, ftout, siz);
                }
            }
        }
    }

    /*
     * Get the BEST ONE!
    */
    double min_em = 9999.0f;
    struct __clssf best_cls;
    for(int i = s ; i <= sh; i += s )
    {
        for(int j = t; j <= sw; j += t )
        {


            for(int y = 0; y < sh - i + 1; y ++ )
            {
                for(int x = 0; x < sw - j + 1; x ++)
                {
                    char file[256] = {0};
                    sprintf(file, "%s/clssf_%d_%d_%d_%d_%d_%d", CLASSFIER_PATH, x, y, j, i, tt, tn);

                    struct __clssf cls;

                    read_file(file, (char*)(&cls), sizeof(struct __clssf));
                    if(min_em > cls.em) {
                        min_em = cls.em;
                        memcpy(&best_cls, &cls, sizeof(struct __clssf));
                    }
                }
            }
        }
    }
    printf("best best_cls is em: %f  am: %f\n", best_cls.em, best_cls.am);

    double em = best_cls.em;
    double am = best_cls.am;
    double zm = 0.0f;
    for(int i = 0; i < siz; i ++) {
        zm += g_weight[i] * exp(-am * ftout[i]);
    }

    for(int i = 0; i < siz; i ++) {
        g_weight[i] = g_weight[i] * exp(-am * ftout[i]) / zm;
    }

    printf("\n");
    write_file(WEIGHT_FILE, (char*)g_weight, siz * sizeof(double));


    char file[256] = {0};
    sprintf(file, "%s/wk_clssf_%d_%d_%d_%d_%d_%d_%d", WEAK_CLASSFIER_PATH,
                rt, best_cls.x, best_cls.y, best_cls.bw, best_cls.bh, best_cls.tt, best_cls.tn);
    write_file(file, (char*)(&best_cls), sizeof(struct __clssf));
}
