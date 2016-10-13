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
std::vector<struct __possi_rect*> g_possi_rects;

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
        if(k == 27) break;
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

    /************
    * (1) Init all weights value averangely
    *********/
    int weight_size = nw * nh * 2;

    double* weights = (double*) malloc(weight_size * sizeof(double));
    int* loss = (int*) malloc(weight_size * sizeof(int));

    double init_weight = 1.0f / ((double)weight_size);
    for(int i = 0; i < weight_size; i ++) {
        weights[i] = init_weight;
    }

    /************
    * (2) Get Possitive & Negative 2 kinds Samples' integral maps
    ***************/
    integ1 = (u32*) malloc(w * h * sizeof(u32));
    integ2 = (u32*) malloc(w * h * sizeof(u32));

    g_haarlike.haarlike_integral(sample1, integ1, w, h);
    g_haarlike.haarlike_integral(sample2, integ2, w, h);



    /*************
     * (3) Get all feature values with integral maps
    ***************/
    const int __temp_kinds = 5;
    int (*_p_fv_creater_arr[__temp_kinds]) (
            u32 *integ, u32 w, u32 h,
            u32 sw, u32 sh,
            u8 x, u8 y, u8 bw, u8 bh, int **ftout) =
    {
        g_haarlike.haarlike_edge_vert,
        g_haarlike.haarlike_edge_horizon,
        g_haarlike.haarlike_linear_vert,
        g_haarlike.haarlike_linear_horizon,
        g_haarlike.haarlike_point
    };

    u8 _t_arr[__temp_kinds] = {1, 2, 1, 3, 2};
    u8 _s_arr[__temp_kinds] = {2, 1, 3, 1, 2};

    for(int temp = 0; temp < __temp_kinds; temp ++)
    {
        temp = 4;
        t = _t_arr[temp];
        s = _s_arr[temp];

        for(int i = s ; i <= sh; i += s )
        {
            for(int j = t; j <= sw; j += t )
            {
                for(int y = 0; y < sh - i + 1; y ++ )
                {
                    for(int x = 0; x < sw - j + 1; x ++)
                    {
                        int *ftout = NULL;
                        int siz1 = _p_fv_creater_arr[temp](integ1, w, h, sw, sh, x, y, j, i, &ftout1);
                        int siz2 = _p_fv_creater_arr[temp](integ2, w, h, sw, sh, x, y, j, i, &ftout2);
                        int siz = siz1 + siz2;

                        ftout = (int*) malloc(siz * sizeof(int));

                        memcpy(ftout, ftout1, siz1 * sizeof(int));
                        memcpy(ftout + siz1, ftout2, siz2 * sizeof(int));

#if 0
                        if(temp == 4)
                        {
                            for(int n = 0; n < siz; n ++)
                            {
                                printf("%d ", ftout[n]);
                            }
                            printf("============================================\n");
                        }

#endif
                        char file[256] = {0};
                        sprintf(file, "%s/f_v_%d_%d_%d_%d_%d_%d", FEATURE_PATH, x, y, j, i, 0, 0);
                        write_file(file, (char*)ftout, siz * sizeof(int));

                        struct __clssf *cls = create_clssfier(x, y, j, i, 0, 0, siz);
                        g_classifiers.push_back(cls);

                        free(ftout);
                        free(ftout1);
                        free(ftout2);
                    }
                }
            }
        }
    }

    /*********
    * Do limits numbers' iteration, get all Weak Classifiers
    * Combine all weaks to a strong
    **************/
    if(g_strocls == 0) {
        g_strocls = new __strong_clssf();
    }

    for(int rt = 0; rt < TRAINING_TIMES; rt ++) {

        struct __clssf* wkcls = create_next_weak_clss(weights, loss, weight_size);

        recalculate_weights(wkcls, weights, loss);

#if 0
        for(int i = 0; i < weight_size; i ++) {

            printf("%f ", weights[i]);

        }
        printf("\n");
#endif

        g_strocls->vwkcs.push_back(wkcls);
        g_strocls->nwk ++;

        printf("the %d weak classifier created [%f  %f]  %d [%d%d]\n", rt, wkcls->am, wkcls->em, wkcls->thrhd, wkcls->tt, wkcls->tn);
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
        //nblock = fwrite(p, sizeof(struct __clssf), 1, fd);
        int x = (int)p->x;
        int y = (int)p->y;
        int bw = (int)p->bw;
        int bh = (int)p->bh;
        int tt = (int)p->tt;
        int tn = (int)p->tn;
        int ss = p->ss;
        int thrhd = p->thrhd;
        double am = p->am;
        double em = p->em;

        fwrite(&x, sizeof(int), 1, fd);
        fwrite(&y, sizeof(int), 1, fd);
        fwrite(&bw, sizeof(int), 1, fd);
        fwrite(&bh, sizeof(int), 1, fd);
        fwrite(&tt, sizeof(int), 1, fd);
        fwrite(&tn, sizeof(int), 1, fd);
        fwrite(&ss, sizeof(int), 1, fd);
        fwrite(&thrhd, sizeof(int), 1, fd);
        fwrite(&am, sizeof(double), 1, fd);
        fwrite(&em, sizeof(double), 1, fd);

        printf("[%d %d %d %d %d %d %d %d %f %f]\n", x, y, bw, bh, tt, tn, ss, thrhd, am, em);

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
        struct __clssf *p = (struct __clssf*) malloc(sizeof(struct __clssf));
        //int siz = fread(pcls, sizeof(struct __clssf), 1, fd);
        int x = 0;
        int y = 0;
        int bw = 0;
        int bh = 0;
        int tt = 0;
        int tn = 0;
        int ss = 0;
        int thrhd = 0;
        double am = 0.0f;
        double em = 0.0f;

        fread(&x, sizeof(int), 1, fd);
        fread(&y, sizeof(int), 1, fd);
        fread(&bw, sizeof(int), 1, fd);
        fread(&bh, sizeof(int), 1, fd);
        fread(&tt, sizeof(int), 1, fd);
        fread(&tn, sizeof(int), 1, fd);
        fread(&ss, sizeof(int), 1, fd);
        fread(&thrhd, sizeof(int), 1, fd);
        fread(&am, sizeof(double), 1, fd);
        fread(&em, sizeof(double), 1, fd);

        p->x = x;
        p->y = y;
        p->bw = bw;
        p->bh = bh;
        p->tt = tt;
        p->tn = tn;
        p->ss = ss;
        p->thrhd = thrhd;
        p->am = am;
        p->em = em;

        //printf("[%d %d %d %d %d %d %d %d %f %f]\n", x, y, bw, bh, tt, tn, ss, thrhd, am, em);

        (*stroclssf)->vwkcs.push_back(p);
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
    struct __clssf* p_best = (struct __clssf*)malloc(sizeof(struct __clssf));
    int counter = 0;
    int *ftout = 0;
    int * __loss = (int*) malloc(sizeof(int) * length);

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

        char file[256] = {0};
        sprintf(file, "%s/f_v_%d_%d_%d_%d_%d_%d", FEATURE_PATH, x, y, bw, bh, tt, tn);
        read_file(file, (char*)ftout, length * sizeof(int));

        statistic_clssfier(p, ftout, p->ss, weights, __loss);

        if(min_em > p->em) {
            min_em = p->em;
            memcpy(p_best, p, sizeof(struct __clssf));
            memcpy(loss, __loss, sizeof(int) * length);
        }

        free(ftout);
    }

    printf("weak classifier %d %d %d %d  \n", p_best->x, p_best->y, p_best->bw, p_best->bh);

    return p_best;
}


static int get_rect_face_fv( u32* integral,
                             int w, int h,
                             int wx, int wy,
                             int wblock,
                             int x, int y,
                             int block_width, int block_height,
                             int template_type, int template_index,
                             int sample_block ) {

    int _x = 0;
    int _y = 0;
    int _bw = 0;
    int _bh = 0;
    _x = x * (wblock / sample_block);
    _y = y * (wblock / sample_block);

    _bw = block_width * (wblock / sample_block);
    _bh = block_height * (wblock / sample_block);
    int *ftout = 0;
    int fv = 0;

    switch(template_type) {
    case 0:
        switch(template_index) {
        case 0:
            g_haarlike.haarlike_edge_horizon(integral, w, h,
                                  w, h, _x, _y, _bw, _bh, &ftout);
            fv = ftout[0] / (wblock / sample_block);
            free(ftout);
            return fv;

        break;
        case 1:
            g_haarlike.haarlike_edge_vert(integral, w, h,
                                  w, h, _x, _y, _bw, _bh, &ftout);
            fv = ftout[0] / (wblock / sample_block);

            free(ftout);
            return fv;
        /// TODO: implement new ones
        break;
        }
    case 1:
        g_haarlike.haarlike_point(integral, w, h,
                                  w, h, _x, _y, _bw, _bh, &ftout);
        fv = ftout[0] / (wblock / sample_block);
        free(ftout);
        return fv;
    break;

    }
}

static int adaboost_jump_block_face(u32* integral, int w, int h,
                                    int wx, int wy, int wblock, __strong_clssf* pstr_clss) {
    double score = 0.0f;
    double scoreav = 0.0f;

    std::vector<struct __clssf*>::iterator it;
    for( it = pstr_clss->vwkcs.begin(); it != pstr_clss->vwkcs.end(); it ++ ) {
        struct __clssf* p = *it;
        int fv = get_rect_face_fv(integral, w, h,
                                    wx, wy, wblock,
                                    p->x, p->y, p->bw, p->bh, p->tt, p->tn, SAMPLE_BLOCK_WIDTH);

        int rt = fv >= p->thrhd ? 1 : 0;
        score += (double)rt * (p->am);
        scoreav += 0.5f * p->am;
        //printf("%d, %f     %f [%d  %d]\n", rt, score, scoreav, p->thrhd, fv);
    }
    printf("score: %f        scoreav: %f \n",  score, scoreav);
    if(score > scoreav) {
        return 1;
    }
    return 0;
}

int adaboost_mark_face(u8 *origin, int iw, double poss, int x, int y, int w, int h) {

    for(int i = y; i < y + h; i ++) {
        for(int j = x; j < x + w; j ++) {
            int index = i * iw + j;
            origin[index] = 255;
        }
    }
}

int adaboost_single_face_judge(u8 *image, int block)
{
    u32* integral = (u32*) malloc(sizeof(u32) * block * block);
    g_haarlike.haarlike_integral(image, integral, block, block);
    static __strong_clssf* pstr_clss = 0;

    if(pstr_clss == 0)
    {
        load_strong_clss_from_file(STRONG_CLASSFIER_FILE, &pstr_clss);
    }
    return adaboost_jump_block_face(integral, block, block, 0, 0, block, pstr_clss);
}



void get_block_data(u8* src, int w, int h, u8* dst, int x, int y, int block) {

    for(int i = y, i2 = 0; i < y + block; i ++, i2 ++) {
        for(int j = x, j2 = 0; j < x + block; j ++, j2 ++) {
            dst[i2 * block + j2] = src[i * w + j];
        }
    }
}

int adaboost_face_test(u8 *image, u8 *origin, int w, int h) {

    /**
     * (1) get integral image
    */

    int window_size = SAMPLE_BLOCK_WIDTH;
    int jump_x = 0;
    int jump_y = 0;
    int x = 0;
    int y = 0;
    int rt = 0;
    int n = 0;


    static __strong_clssf* pstr_clss = 0;

    if(pstr_clss == 0) {
        load_strong_clss_from_file(STRONG_CLASSFIER_FILE, &pstr_clss);
    }

    g_possi_rects.clear();

    window_size = 60;

    u8 *imgb = (u8*)malloc(window_size * window_size);
    //u32* integral = (u32*) malloc(sizeof(u32) * window_size * window_size);

    int normal_jump = 10;
    x = y = 0;
    jump_x = (x + normal_jump) + window_size >= w ? (w - x - window_size) : normal_jump;
    jump_y = (y + normal_jump) + window_size >= h ? (h - y - window_size) : normal_jump;

    for(y = 0; y + window_size <= h - 1; y += jump_y) {
        for(x = 0; x + window_size <= w - 1; x += jump_x) {

            get_block_data(image, w, h, imgb, x, y, window_size);
            //rt = adaboost_jump_block_face(integral, w, h, x, y, window_size, pstr_clss);

            rt = adaboost_single_face_judge(imgb, window_size);

            if(rt) {
                struct __possi_rect* prt = (struct __possi_rect*)malloc(sizeof(struct __possi_rect));
                prt->x = x;
                prt->y = y;
                prt->w = window_size;
                prt->h = window_size;
                g_possi_rects.push_back(prt);
                //adaboost_mark_face(origin, w, rt, x, y, window_size, window_size);
            }
        }
    }

    printf("find %d faces \n", g_possi_rects.size());
    //window_size += 1;

#if 0
    do {
        int normal_jump = (window_size >> 2);
        x = y = 0;
        jump_x = (x + normal_jump) + window_size >= w ? (w - x - window_size) : normal_jump;
        jump_y = (y + normal_jump) + window_size >= h ? (h - y - window_size) : normal_jump;

        for(y = 0; y + window_size <= h - 1; y += jump_y) {
            for(x = 0; x + window_size <= w - 1; x += jump_x) {


                rt = adaboost_jump_block_face(integral, w, h, x, y, window_size, pstr_clss);


                if(rt) {
                    struct __possi_rect* prt = (struct __possi_rect*)malloc(sizeof(struct __possi_rect));
                    prt->x = x;
                    prt->y = y;
                    prt->w = window_size;
                    prt->y = window_size;
                    g_possi_rects.push_back(prt);
                    //adaboost_mark_face(origin, w, rt, x, y, window_size, window_size);
                }
            }
        }

        window_size += 1;
    }
    while(0);
#endif
    //while(window_size <= (w >> 1));

    return 0;
    //
}
