#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include "proc.h"
#include "proc_utils.h"

#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

int statistic_clssfier(struct __clssf* cls, struct __feature_value** fv,
                                int siz, double* global_weight, int* global_loss);
struct __clssf* create_clssfier(int x, int y, int bw, int bh, int tt, int tn, int siz);
extern HaarlikeProc g_haarlike;

int up_sample_from = 0;
int up_sample_to = 1999;
int down_sample_from = 2000;
int down_sample_to = 3999;
int max_number_samples = 4000;

double*                      g_weights = NULL;
int*                         g_loss = NULL;
std::vector<struct __clssf*> g_classifiers;
__strong_clssf*              g_strocls = NULL;

std::vector<struct __possi_rect*> g_possi_rects;

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

#if 0
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
    u8 _tt_arr[__temp_kinds] = {0, 1, 2, 3, 4};

    for(int temp = 0; temp < __temp_kinds; temp ++)
    {
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
                        sprintf(file, "%s/f_v_%d_%d_%d_%d_%d_%d", FEATURE_PATH, x, y, j, i, _tt_arr[temp], 0);
                        write_file(file, (char*)ftout, siz * sizeof(int));

                        struct __clssf *cls = create_clssfier(x, y, j, i, _tt_arr[temp], 0, siz);
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
#endif

int save_clssf_index()
{
    FILE* fd = fopen(CLSSF_INDEX_FILE, "w");
    int len = g_classifiers.size();

    fwrite(&len, sizeof(int), 1, fd);

    std::vector<struct __clssf*>::iterator it;
    for( it = g_classifiers.begin(); it != g_classifiers.end(); it ++ )
    {
        struct __clssf* p = *it;
        int x = (int)p->x;
        int y = (int)p->y;
        int bw = (int)p->bw;
        int bh = (int)p->bh;
        int tt = (int)p->tt;
        int tn = (int)p->tn;
        int siz = (int)p->ss;
        fwrite(&x, sizeof(int), 1, fd);
        fwrite(&y, sizeof(int), 1, fd);
        fwrite(&bw, sizeof(int), 1, fd);
        fwrite(&bh, sizeof(int), 1, fd);
        fwrite(&tt, sizeof(int), 1, fd);
        fwrite(&tn, sizeof(int), 1, fd);
        fwrite(&siz, sizeof(int), 1, fd);
    }
    fflush(fd);
    fclose(fd);
}

int load_clssf_index()
{
    FILE* fd = fopen(CLSSF_INDEX_FILE, "r");

    g_classifiers.clear();

    int _size = 0;
    fread(&_size, sizeof(int), 1, fd);

    for(int i = 0; i < _size; i ++)
    {
        int x = 0;
        int y = 0;
        int bw = 0;
        int bh = 0;
        int tt = 0;
        int tn = 0;
        int siz = 0;
        fread(&x, sizeof(int), 1, fd);
        fread(&y, sizeof(int), 1, fd);
        fread(&bw, sizeof(int), 1, fd);
        fread(&bh, sizeof(int), 1, fd);
        fread(&tt, sizeof(int), 1, fd);
        fread(&tn, sizeof(int), 1, fd);
        fread(&siz, sizeof(int), 1, fd);

        struct __clssf *cls = create_clssfier(x, y, bw, bh, tt, tn, siz);
        g_classifiers.push_back(cls);
    }

    fflush(fd);
    fclose(fd);
}

int save_weak_clss_to_file( const char* file, struct __clssf* weak_clssf)
{
    FILE* fd = fopen(file, "w");

    struct __clssf* p = weak_clssf;
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

    fflush(fd);
    fclose(fd);

    return 0;
}

int load_weak_clss_from_file( const char* file, struct __clssf** weak_clssf )
{
    if(!weak_clssf)
    {
        return -1;
    }

    FILE* fd = fopen(file, "r");

    *weak_clssf = (struct __clssf*)malloc(sizeof(struct __clssf));
    struct __clssf *p = *weak_clssf;

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

    fclose(fd);
}


int save_strong_clss_to_file(__strong_clssf* stroclssf) {

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

        printf("[%d %d %d %d [%d] %d %d %d %f %f]\n", x, y, bw, bh, tt, tn, ss, thrhd, am, em);

    }

    fflush(fd);
    fclose(fd);

    return 0;
}

int load_strong_clss_from_file(const char* file, __strong_clssf** stroclssf) {

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

int load_fv_from_file(const char* file, struct __feature_value** fv_array, int len)
{
    if(!fv_array)
    {
        return -1;
    }
    int* fvs = (int*) malloc(len * sizeof(int));
    read_file(file, (char*)fvs, len * sizeof(int));

    for(int i = 0; i < len; i ++)
    {
        fv_array[i] = (struct __feature_value*)malloc(sizeof(struct __feature_value));
        fv_array[i]->value = fvs[i];
        fv_array[i]->sample_id = i;
    }

    free(fvs);
    return 0;
}

struct __clssf* create_clssfier(int x, int y, int bw, int bh, int tt, int tn, int siz) {
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


static int find_clssfier_threhd_loss(struct __feature_value** fv_arr, int siz,
                    double* global_weight,
                    int* global_loss,
                    int* thre, double* em_out, int *tn) {

    int fv_all_size = siz;
#if 1
    double t_up, t_down, s_up, s_down;
    t_up = t_down = s_up = s_down = 0.0f;
    int tmp_thrhd = 0;
    int fv_up_size = up_sample_to - up_sample_from + 1;
    int fv_down_size = down_sample_to - down_sample_from + 1;
    double em_min = 999999.9f;
    double _em = 0.0f;
    double _em1 = 0.0f;
    double _em2 = 0.0f;
    int tmp_tn = 0;
#endif

#if 0
    double em_min = 999999.9f;
    int tmp_tn = 0;



    double em1 = 0.0f;
    double em2 = 0.0f;
    double _em = 0.0f;
    int tmp_thrhd = 0;



#if 1
    for(int i = 0; i < fv_all_size; i ++)
    {
        int tt = fv_arr[i]->value;
#else
    int min_val = 99999999;
    int max_val = -9999999;
    int max_min = max_val - min_val;
    double last_em = 0.0f;
    int left = min_val;
    int right = max_val;
    int last = 0;
    for(int i = 0; i < fv_all_size; i ++)
    {
        int v = fv_arr[i]->value;
        max_val = v > max_val ? v : max_val;
        min_val = v < min_val ? v : min_val;
    }
    for(int i = min_val; i < max_val; i = (left + right) / 2)
    {
        int tt = i;
#endif

        for(int j = 0; j < fv_all_size; j ++)
        {
            /// + template em1
            if(j <= up_sample_to && fv_arr[j]->value < tt)
            {
                em1 += global_weight[j];
            }
            else if(j >= down_sample_from && fv_arr[j]->value > tt)
            {
                em1 += global_weight[j];
            }


            /// - template em2
            if(j <= up_sample_to && fv_arr[j]->value > tt)
            {
                em2 += global_weight[j];
            }
            else if(j >= down_sample_from && fv_arr[j]->value < tt)
            {
                em2 += global_weight[j];
            }
        }

        _em = em1 > em2 ? em2 : em1;

        if(_em < em_min)
        {
            tmp_thrhd = tt;
            em_min = _em;
            tmp_tn = em1 < em2 ? 0 : 1;
        }
    }

    /// for loss
    for(int i = 0; i < fv_all_size; i ++)
    {
        global_loss[i] = 1;
        if(tmp_tn == 0)
        {
            if(i <= up_sample_to)
            {
                if(fv_arr[i]->value < tmp_thrhd)
                {
                    global_loss[i] = -1;
                }
            }
            else if(i >= down_sample_from)
            {
                if(fv_arr[i]->value > tmp_thrhd)
                {
                    global_loss[i] = -1;
                }
            }
        }
        else if(tmp_tn == 1)
        {
            if(i <= up_sample_to)
            {
                if(fv_arr[i]->value > tmp_thrhd)
                {
                    global_loss[i] = -1;
                }
            }
            else if(i >= down_sample_from)
            {
                if(fv_arr[i]->value < tmp_thrhd)
                {
                    global_loss[i] = -1;
                }
            }
        }

    }
#endif
#if 1

    struct __feature_value** all_nfvs = (struct __feature_value**)malloc(fv_all_size * sizeof(struct __feature_value*));

    double *_up_wt_integral = (double*) malloc(fv_up_size * sizeof(double));
    double *_down_wt_integral = (double*) malloc(fv_down_size * sizeof(double));

    sort_fv_by_val(fv_arr, up_sample_from, up_sample_to);
    sort_fv_by_val(fv_arr, down_sample_from, down_sample_to);

    /// reset index
    for(int i = up_sample_from; i <= up_sample_to; i ++)
    {
        fv_arr[i]->index = i;
        fv_arr[i]->up_down = 1;
    }
    for(int i = down_sample_from, j = 0; i <= down_sample_to; i ++, j ++)
    {
        fv_arr[i]->index = j;
        fv_arr[i]->up_down = -1;
    }


    for(int i = 0; i < fv_all_size; i ++)
    {
        all_nfvs[i] = (struct __feature_value*)malloc(sizeof(struct __feature_value));
        all_nfvs[i]->index = fv_arr[i]->index;
        all_nfvs[i]->sample_id = fv_arr[i]->sample_id;
        all_nfvs[i]->value = fv_arr[i]->value;
        all_nfvs[i]->up_down = fv_arr[i]->up_down;
    }
    sort_fv_by_val(all_nfvs, 0, fv_all_size - 1);

    /**
     * get weight values integral maps
    */
    for(int i = 0; i < fv_up_size; i ++)
    {
        double weight = global_weight[fv_arr[i]->sample_id];
        _up_wt_integral[i] = i > 0 ? _up_wt_integral[i - 1] + weight : weight;
    }

    for(int i = 0, j = i + down_sample_from; i < fv_down_size; i ++, j ++)
    {
        double weight = global_weight[fv_arr[j]->sample_id];
        _down_wt_integral[i] = i > 0 ? _down_wt_integral[i - 1] + weight :
                                       weight;
    }

    t_up = _up_wt_integral[fv_up_size - 1];
    t_down = _down_wt_integral[fv_down_size - 1];
    s_up = -9999.0f;
    s_down = -9999.0f;

    /**
     * Go over all value for threhold
    */
    if(fv_up_size != fv_down_size)
    {
        printf("[Exception] Code not implemented %s:%d\n", __FILE__, __LINE__);
        return -1;
    }

    for(int i = 0; i < fv_all_size; i ++)
    {
        int tt = all_nfvs[i]->value;
        int tt2 = 0;
        if(all_nfvs[i]->up_down == 1)
        {
            s_up = _up_wt_integral[all_nfvs[i]->index];
            int find = 0;
            /// back search
            for(int j = i; j >= 0; j --)
            {
                if(all_nfvs[j]->up_down == -1)
                {
                    s_down = _down_wt_integral[all_nfvs[j]->index];
                    find = 1;
                    tt2 = all_nfvs[j]->value;
                    break;
                }
            }
            /// front search
            if(!find)
            {
                for(int j = i; j < fv_all_size; j ++)
                {
                    if(all_nfvs[j]->up_down == -1)
                    {
                        s_down = _down_wt_integral[all_nfvs[j]->index];
                        find = 1;
                        tt2 = all_nfvs[j]->value;
                        break;
                    }
                }

            }
        }
        else if(all_nfvs[i]->up_down == -1)
        {
            s_down = _down_wt_integral[all_nfvs[i]->index];
            int find = 0;
            /// back search
            for(int j = i; j >= 0; j --)
            {
                if(all_nfvs[j]->up_down == 1)
                {
                    s_up = _up_wt_integral[all_nfvs[j]->index];
                    find = 1;
                    tt2 = all_nfvs[j]->value;
                    break;
                }
            }
            /// front search
            if(!find)
            {
                for(int j = i; j < fv_all_size; j ++)
                {
                    if(all_nfvs[j]->up_down == 1)
                    {
                        s_up = _up_wt_integral[all_nfvs[j]->index];
                        find = 1;
                        tt2 = all_nfvs[j]->value;
                        break;
                    }
                }
            }
        }
        if(s_up < -9998.0f || s_down < -9998.0f)
        {
            printf("[HUGE ERROR] %f %f\n", s_up, s_down);
            return -1;
        }

        double em1 = s_up + (t_down - s_down);
        double em2 = s_down + (t_up - s_up);

        double _em = em1 > em2 ? em2 : em1;

        if(_em < em_min)
        {
            em_min = _em;
            tmp_tn = em1 < em2 ? 0 : 1;
            _em1 = em1;
            _em2 = em2;
            tmp_thrhd = (tt + tt2) / 2;
        }
    }


    for(int i = 0; i < fv_all_size; i ++)
    {
        global_loss[i] = 0;
    }

    for(int i = 0; i < fv_all_size; i ++)
    {
        int sample_id = fv_arr[i]->sample_id;

        global_loss[sample_id] = 1;

        if(tmp_tn == 0)
        {
            if(fv_arr[i]->up_down == 1)
            {
                if(fv_arr[i]->value < tmp_thrhd)
                {
                    global_loss[sample_id] = -1;
                }
            }
            else if(fv_arr[i]->up_down == -1)
            {
                if(fv_arr[i]->value > tmp_thrhd)
                {
                    global_loss[sample_id] = -1;
                }
            }
        }
        else if(tmp_tn == 1)
        {
            if(fv_arr[i]->up_down == 1)
            {
                if(fv_arr[i]->value > tmp_thrhd)
                {
                    global_loss[sample_id] = -1;
                }
            }
            else if(fv_arr[i]->up_down == -1)
            {
                if(fv_arr[i]->value < tmp_thrhd)
                {
                    global_loss[sample_id] = -1;
                }
            }

        }

    }

    for(int i = 0; i < fv_all_size; i ++)
    {
        if(global_loss[i] == 0)
        {
            printf("\n\n[ERROR] HUGE ERROR %d %d is 0\n\n", i, global_loss[i]);
        }

    }

    free(_up_wt_integral);
    free(_down_wt_integral);

    for(int i = 0; i < fv_all_size; i ++)
    {
        free(all_nfvs[i]);
    }
    free(all_nfvs);
#endif

    *thre = tmp_thrhd;
    *em_out = em_min;
    *tn = tmp_tn;

    return 0;
}

int recalculate_weights(struct __clssf* wkclsf, double* weights, int* loss) {
    double em = wkclsf->em;
    double am = wkclsf->am;
    double ex = 0.0f;
    double zm = 0.0f;
    double wp = 0.0f;

    for(int i = 0; i < wkclsf->ss; i ++) {
        zm += weights[i] * exp(-am * (double)loss[i]);
    }

    for(int i = 0; i < wkclsf->ss; i ++) {
        ex = exp(-am * (double)loss[i]);
        weights[i] = weights[i] * ex / zm;
    }
    for(int i = 0; i < wkclsf->ss; i ++)
    {
        wp += weights[i];
    }
    printf("wp is: %f\n", wp);
}

int statistic_clssfier(struct __clssf* cls, struct __feature_value** fv,
                                int siz, double* global_weight, int* global_loss) {

    double em = 0.0f;
    double am = 0.0f;
    int wrong = 0;

    int threhold = 0;
    int tn = 0;

    find_clssfier_threhd_loss(fv, siz, global_weight, global_loss, &threhold, &em, &tn);

    cls->thrhd = threhold;
    cls->tn = tn;

    double en = (1.0f - em) / em;
    double es = log(en);
    am = 0.5 * es;

    cls->em = em;
    cls->am = am;

    return 0;
}


struct __clssf* create_next_weak_clss(double* weights, int* loss, int length) {

    /*
     * Redo statistics and
     * Get the BEST ONE!
    */
    double min_em = 9999999.0f;
    int *temp_loss = (int*)malloc(length * sizeof(int));
    memset(temp_loss, 0, length * sizeof(int));

    struct __clssf* p_best = (struct __clssf*)malloc(sizeof(struct __clssf));

    int counter = 0;


    std::vector<struct __clssf*>::iterator it_erase;
    std::vector<struct __clssf*>::iterator it;
    for( it = g_classifiers.begin(); it != g_classifiers.end(); it ++ ) {

        struct __clssf* p = *it;

        int x = p->x;
        int y = p->y;
        int bw = p->bw;
        int bh = p->bh;
        int tt = p->tt;
        int tn = 0;


        struct __feature_value** fv_arr =
            (struct __feature_value**)malloc(length * sizeof(struct __feature_value*));

        char file1[256] = {0};
        sprintf(file1, "%s/f_v_%d_%d_%d_%d_%d_%d", FEATURE_PATH, x, y, bw, bh, tt, tn);
        load_fv_from_file(file1, fv_arr, length);


        statistic_clssfier(p, fv_arr, length, weights, temp_loss);

        //if(counter % 10000 == 0) {
        //    printf("[%d] %d %f %d \n", counter, p->thrhd, p->em, p->tn);
        //}
        //printf("[%d] %d %f %d \n", counter, p->thrhd, p->em, p->tn);

        if(min_em > p->em) {
            memcpy(loss, temp_loss, length * sizeof(int));
            memcpy(p_best, p, sizeof(struct __clssf));
            min_em = p->em;
            it_erase = it;
            printf("Find Smaller: [%d] %d %f %d \n", counter, p->thrhd, p->em, p->tn);
        }

        for(int fv_n = 0; fv_n < length; fv_n ++)
        {
            free(fv_arr[fv_n]);
        }
        free(fv_arr);
        counter ++;
    }



    min_em = p_best->em;

    printf("weak classifier %d %d %d %d  \n", p_best->x, p_best->y, p_best->bw, p_best->bh);

    g_classifiers.erase(it_erase);

    return p_best;
}
