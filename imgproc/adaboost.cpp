#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include "proc.h"
#include "proc_utils.h"

#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

static int statistic_clssfier(struct __clssf* cls, int *ftout, int siz, double* weight, int* loss);
static struct __clssf*  create_next_weak_clss(double* weights, int* loss, int len);
static struct __clssf* create_clssfier(int x, int y, int bw, int bh, int tt, int tn, int siz);
static int recalculate_weights(struct __clssf* wkclsf, double* weight, int* loss);
static int save_strong_clss_to_file(__strong_clssf* stroclssf);

extern HaarlikeProc g_haarlike;
static double *g_weight = 0;
static __strong_clssf* g_strocls = 0;
static int up_sample_from = 0;
static int up_sample_to = 1999;
static int down_sample_from = 2000;
static int down_sample_to = 3999;

static std::vector<struct __clssf*> g_classifiers;

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


int adaboost_train(u8* sample1, u8* sample2, int w, int h, int sw, int sh)
{
    int s = 1;
    int t = 2;

    int *ftout1 = 0;
    int *ftout2 = 0;
    u32 wf = 0;
    u32 hf = 0;
    u32 *integ1 = 0;
    u32 *integ2 = 0;

    int bw, bh;

    int nw = w / sw;
    int nh = h / sh;

    int weight_size = nw * nh * 2;

    double* weights = (double*) malloc(weight_size * sizeof(double));
    int* loss = (int*) malloc(weight_size * sizeof(int));

    double init_weight = 1.0f / ((double)weight_size);
    for(int i = 0; i < weight_size; i ++) {
        weights[i] = init_weight;
    }
    //write_file(WEIGHT_FILE, (char*)weight, weight_size * sizeof(double));

    integ1 = (u32*) malloc(w * h * sizeof(u32));
    integ2 = (u32*) malloc(w * h * sizeof(u32));

    g_haarlike.haarlike_integral(sample1, integ1, w, h);
    g_haarlike.haarlike_integral(sample2, integ2, w, h);

    /*
     * Edge 0 & 1
    */
    s = 4;
    t = 2;

    int counter = 0;
    for(int i = s ; i <= sh; i += s ) {

        for(int j = t; j <= sw; j += t ) {

            for(int y = 0; y < sh - i + 1; y ++ ) {
                for(int x = 0; x < sw - j + 1; x ++) {
                    int *ftout = NULL;
                    int siz1 = g_haarlike.haarlike_edge_horizon(integ1, w, h, sw, sh, x, y, j, i, &ftout1);
                    int siz2 = g_haarlike.haarlike_edge_horizon(integ2, w, h, sw, sh, x, y, j, i, &ftout2);
                    int siz = siz1 + siz2;
#if 0
                    for(int n = 0; n < siz; n ++) {
                        printf("%d ", ftout[n]);
                    }
                    printf("============================================\n");
#endif
                    ftout = (int*) malloc(siz * sizeof(int));

                    memcpy(ftout, ftout1, siz1 * sizeof(int));
                    memcpy(ftout + siz1, ftout2, siz2 * sizeof(int));

                    char file[256] = {0};
                    sprintf(file, "%s/f_v_%d_%d_%d_%d_%d_%d", FEATURE_PATH, x, y, j, i, 0, 0);
                    write_file(file, (char*)ftout, siz * sizeof(int));

                    struct __clssf *cls = create_clssfier(x, y, j, i, 0, 0, siz);
                    g_classifiers.push_back(cls);

                    free(ftout);
                    free(ftout1);
                    free(ftout2);

                    counter ++;
                    //printf("[counter %d]\n", counter);
                }
            }
        }
    }


    s = 2;
    t = 4;
    for(int i = s ; i <= sh; i += s ) {

        for(int j = t; j <= sw; j += t ) {


            for(int y = 0; y < sh - i + 1; y ++ ) {
                for(int x = 0; x < sw - j + 1; x ++) {

                    int *ftout = NULL;
                    int siz1 = g_haarlike.haarlike_edge_horizon(integ1, w, h, sw, sh, x, y, j, i, &ftout1);
                    int siz2 = g_haarlike.haarlike_edge_horizon(integ2, w, h, sw, sh, x, y, j, i, &ftout2);
                    int siz = siz1 + siz2;
#if 0
                    for(int n = 0; n < siz; n ++) {
                        printf("%d ", ftout[n]);
                    }
                    printf("============================================\n");
#endif
                    ftout = (int*) malloc(siz * sizeof(int));

                    memcpy(ftout, ftout1, siz1 * sizeof(int));
                    memcpy(ftout + siz1, ftout2, siz2 * sizeof(int));

                    char file[256] = {0};
                    sprintf(file, "%s/f_v_%d_%d_%d_%d_%d_%d", FEATURE_PATH, x, y, j, i, 0, 1);
                    write_file(file, (char*)ftout, siz * sizeof(int));

                    struct __clssf *cls = create_clssfier(x, y, j, i, 0, 1, siz);
                    g_classifiers.push_back(cls);

                    free(ftout);
                    free(ftout1);
                    free(ftout2);
                    counter ++;
                    //printf("[counter %d]\n", counter);
                }
            }
        }
    }
    //printf("[Classifier all indexes created  %d]\n", g_classifiers.size());
    if(g_strocls == 0) {
        g_strocls = new __strong_clssf();
    }

    for(int rt = 0; rt < TRAINING_TIMES; rt ++) {

        struct __clssf* wkcls = create_next_weak_clss(weights, loss, weight_size);

        recalculate_weights(wkcls, weights, loss);

        for(int i = 0; i < weight_size; i ++) {

            printf("%f ", weights[i]);

        }
        printf("\n");

        g_strocls->vwkcs.push_back(wkcls);
        g_strocls->nwk ++;

        printf("the %d weak classifier created [%f  %f]\n", rt, wkcls->am, wkcls->em);
    }

    save_strong_clss_to_file(g_strocls);

    return 0;
}

static int save_strong_clss_to_file(__strong_clssf* stroclssf) {

    FILE* fd = fopen(STRONG_CLASSFIER_FILE, "w");

    int nblock = fwrite((char*)(&(stroclssf->nwk)), sizeof(int), 1, fd);
    if(nblock != 1) {
        printf("Error, write file error: %s   size: %d \n ", STRONG_CLASSFIER_FILE, nblock);
        fclose(fd);
        return -1;
    }

    std::vector<struct __clssf*>::iterator it;
    for( it = stroclssf->vwkcs.begin(); it != stroclssf->vwkcs.end(); it ++ ) {
        struct __clssf* p = *it;

        nblock = fwrite((char*)p, sizeof(struct __clssf), 1, fd);

        if(nblock != 1) {
            printf("Error, write file error: %s   size: %d \n ", STRONG_CLASSFIER_FILE, nblock);
            fclose(fd);
            return -1;
        }
    }

    fflush(fd);
    fclose(fd);

    return 0;
}

static int load_strong_clss_from_file(const char* file, __strong_clssf** stroclssf) {

    if(!stroclssf) {
        return -1;
    }


    FILE* fd = fopen(file, "r");

    *stroclssf = new __strong_clssf();

    int nwk = 0;
    fread(&nwk, sizeof(int), 1, fd);

    for(int i = 0; i < nwk; i ++) {
        struct __clssf *pcls = (struct __clssf*) malloc(sizeof(struct __clssf));
        int siz = fread(pcls, sizeof(struct __clssf), 1, fd);

        if(siz != 1) {
            printf("Read Strong File ERROR %s %d\n", file, siz);
            fclose(fd);
            return -1;
        }

        (*stroclssf)->vwkcs.push_back(pcls);
    }

    (*stroclssf)->nwk = nwk;

    fclose(fd);

}

static struct __clssf* create_clssfier(int x, int y, int bw, int bh, int tt, int tn, int siz) {
    struct __clssf* cls = (struct __clssf*) malloc(sizeof(struct __clssf));
    cls->x = x; cls->y = y;
    cls->bw = bw; cls->bh = bh;
    cls->tt = tt;
    cls->tn = tn;
    cls->ss = siz;
    cls->am = 0.0f;
    cls->em = 0.0f;

    return cls;
}

static int find_clssfier_threhd_loss(int *ftout, int siz, double* weight, int* loss, int* thre, double* em) {
    int _threhold = 0;
    double _em = 0;
    double _min_em = 99999.0f;
    int _min = 999999999;
    int _max = -999999999;
    double I = 1.0f;
    int tt = 0;
    int *__loss = (int*) malloc(siz * sizeof(int));

    for(int i = 0; i < siz; i ++) {
        _min = ftout[i] < _min ? ftout[i] : _min;
        _max = ftout[i] > _max ? ftout[i] : _max;
    }

    for(int t = _min; t < _max; t += (_max - _min) < THREHOLD_ACCURATE ? 1 : (_max - _min)/THREHOLD_ACCURATE) {
        tt = t;
        _em = 0.0f;

        for(int i = 0; i < siz; i ++) {
            __loss[i] = 1;
            if(i >= up_sample_from && i <= up_sample_to) {
                if(ftout[i] < tt) {
                    _em += weight[i] * I;
                    __loss[i] = -1;
                }
            }
            else if(i >= down_sample_from && i <= down_sample_to) {
                if(ftout[i] >= tt) {
                    _em += weight[i] * I;
                    __loss[i] = -1;
                }
            }
        }
        if(_em < _min_em) {
            _threhold = tt;
            _min_em = _em;
            memcpy(loss, __loss, siz * sizeof(int));
        }
    }
    free(__loss);

    *thre = _threhold;
    *em = _min_em;

    return 0;
}

static int recalculate_weights(struct __clssf* wkclsf, double* weights, int* loss) {
    double em = wkclsf->em;
    double am = wkclsf->am;
    double ex = 0.0f;
    double zm = 0.0f;

    for(int i = 0; i < wkclsf->ss; i ++) {
        zm += weights[i] * exp(-am * (double)loss[i]);
    }

    for(int i = 0; i < wkclsf->ss; i ++) {
        ex = exp(-am * (double)loss[i]);
        weights[i] = weights[i] * ex / zm;
    }
}

static int statistic_clssfier(struct __clssf* cls, int *ftout, int siz, double* weight, int* loss) {

    double em = 0.0f;
    double am = 0.0f;
    int wrong = 0;

    int threhold = 0;

    find_clssfier_threhd_loss(ftout, siz, weight, loss, &threhold, &em);

    cls->thrhd = threhold;

    double en = (1.0f - em) / em;
    double es = log(en);
    am = 0.5 * es;

    if(em == 0.0f) {
        am = 0.0f;
    }
    cls->em = em;
    cls->am = am;

    return 0;
}

static struct __clssf* create_next_weak_clss(double* weights, int* loss, int length) {


    /*
     * Redo statistics and
     * Get the BEST ONE!
    */
    double min_em = 9999999.0f;
    struct __clssf* p_best;
    int counter = 0;
    int *ftout = 0;

    std::vector<struct __clssf*>::iterator it;
    for( it = g_classifiers.begin(); it != g_classifiers.end(); it ++ ) {

        struct __clssf* p = *it;

        int x = p->x;
        int y = p->y;
        int bw = p->bw;
        int bh = p->bh;
        int tt = p->tt;
        int tn = p->tn;

        ftout = (int*)malloc(length * sizeof(int));
        memset(ftout, 0, length * sizeof(int));

        if(counter == 2224) {
            printf("Crash\n");
        }

        char file[256] = {0};
        sprintf(file, "%s/f_v_%d_%d_%d_%d_%d_%d", FEATURE_PATH, x, y, bw, bh, tt, tn);
        read_file(file, (char*)ftout, length * sizeof(int));

        statistic_clssfier(p, ftout, p->ss, weights, loss);
        //printf("save normal classifier %d\n", counter ++);

        if(min_em > p->em) {
            min_em = p->em;
            p_best = p;
        }

        free(ftout);
    }

    return p_best;
}


static int get_rect_face_fv( u32* integral,
                             int integral_block,
                             int x, int y,
                             int block_width, int block_height,
                             int template_type, int template_index,
                             int sample_block ) {

    int _x = 0;
    int _y = 0;
    int _bw = 0;
    int _bh = 0;
    _x = x * (integral_block / sample_block);
    _y = y * (integral_block / sample_block);
    _bw = block_width * (integral_block / sample_block);
    _bh = block_height * (integral_block / sample_block);
    int *ftout = 0;
    int fv = 0;

    switch(template_type) {
    case 0:
        switch(template_index) {
        case 0:

            g_haarlike.haarlike_edge_horizon(integral, integral_block, integral_block,
                                  1, 1, _x, _y, _bw, _bh, &ftout);
            fv = ftout[0] / (integral_block / sample_block);

            free(ftout);
            return fv;

        break;
        case 1:
            g_haarlike.haarlike_edge_vert(integral, integral_block, integral_block,
                                  1, 1, _x, _y, _bw, _bh, &ftout);
            fv = ftout[0] / (integral_block / sample_block);

            free(ftout);
            return fv;
        /// TODO: implement new ones
        break;
        }

    break;

    }
}

int adaboost_face_test(u8 *image, int image_block) {

    /**
     * (1) get integral image
    */
    double score1 = 0.0f;
    double scoreav1 = 0.0f;
    double score2 = 0.0f;
    double scoreav2 = 0.0f;

    u32* integral = (u32*) malloc(sizeof(u32) * image_block * image_block);
    g_haarlike.haarlike_integral(image, integral, image_block, image_block);

    static __strong_clssf* pstr_clss1 = 0;
    static __strong_clssf* pstr_clss2 = 0;

    if(pstr_clss1 == 0 && pstr_clss2 == 0) {
        load_strong_clss_from_file(STRONG_CLASSFIER_FILE1, &pstr_clss1);
        //load_strong_clss_from_file(STRONG_CLASSFIER_FILE2, &pstr_clss2);
    }


    std::vector<struct __clssf*>::iterator it;
    for( it = pstr_clss1->vwkcs.begin(); it != pstr_clss1->vwkcs.end(); it ++ ) {
        struct __clssf* p = *it;
        int fv = get_rect_face_fv(integral, image_block, p->x, p->y, p->bw, p->bh, p->tt, p->tn, SAMPLE_BLOCK_WIDTH);

        int rt = fv >= p->thrhd ? 1 : 0;
        score1 += (double)rt * (p->am);
        scoreav1 += 0.5f * p->am;
    }
/*
    for( it = pstr_clss2->vwkcs.begin(); it != pstr_clss2->vwkcs.end(); it ++ ) {
        struct __clssf* p = *it;
        int fv = get_rect_face_fv(integral, image_block, p->x, p->y, p->bw, p->bh, p->tt, p->tn, SAMPLE_BLOCK_WIDTH);
        score2 += (double)fv * (p->am);
    }
*/
    //score1 = score1 / pstr_clss1->nwk;
    //score2 = score2 / pstr_clss2->nwk;

    //printf("%f                         %f\n", score1, scoreav1);

    if(score1 > scoreav1) {
        return 1;
    }
    return 0;
}
