#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string.h>

#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

#include "proc.h"
#include "proc_utils.h"

#define __MEMORY_BUFFERED__
#define __FILE_BUFFERED__

extern int up_sample_from;
extern int up_sample_to;
extern int down_sample_from;
extern int down_sample_to;
extern int max_number_samples;

static int sample_width = SAMPLE_BLOCK_WIDTH;
static int sample_height = SAMPLE_BLOCK_WIDTH;

extern HaarlikeProc g_haarlike;

extern double*                      g_weights;
extern int*                         g_loss;
extern std::vector<struct __clssf*> g_classifiers;
extern __strong_clssf*              g_strocls;

struct __clssf* create_clssfier(int x, int y, int bw, int bh, int tt, int tn, int siz);
int statistic_clssfier(struct __clssf* cls, struct __feature_value* fv,
                                int siz, double* global_weight, int* global_loss);
struct __clssf*  create_next_weak_clss(double* weights, int* loss, int len);
int recalculate_weights(struct __clssf* wkclsf, double* weight, int* loss);
int save_strong_clss_to_file(__strong_clssf* stroclssf);
int save_weak_clss_to_file( const char* file, struct __clssf* weak_clssf);
int save_clssf_index();
int load_clssf_index();
int mat_to_u8arr(cv::Mat src, u8* dst, int w, int h);

__MEMORY_BUFFERED__
static int init()
{
    /************
    * (1) Init all weights value averangely
    *********/
    if(g_weights)
    {
        free(g_weights);
        g_weights = NULL;
    }

    g_weights = (double*) malloc(max_number_samples * sizeof(double));

    double init_weight = 1.0f / ((double)max_number_samples);
    for(int i = 0; i < max_number_samples; i ++) {
        g_weights[i] = init_weight;
    }

    /**
     * init g_loss, global loss iterator
    */
    if(g_loss)
    {
        free(g_loss);
        g_loss = NULL;
    }
    g_loss = (int*) malloc(max_number_samples * sizeof(int));

    g_classifiers.clear();


     /**
      * Init strong classifier
     */
    if(g_strocls)
    {
        free(g_strocls);
        g_strocls = NULL;
    }
    if(g_strocls == 0) {
        g_strocls = new __strong_clssf();
    }

    return 0;
}

/************
* (2) Get Possitive & Negative 2 kinds Samples' integral maps
***************/
__MEMORY_BUFFERED__
static int load_samples_get_integral(u8* sample1, u8* sample2, u32** integ1, u32** integ2, int w, int h)
{
    *integ1 = (u32*) malloc(w * h * sizeof(u32));
    *integ2 = (u32*) malloc(w * h * sizeof(u32));

    g_haarlike.haarlike_integral(sample1, *integ1, w, h);
    g_haarlike.haarlike_integral(sample2, *integ2, w, h);
}


/*************
     * (3) Get all feature values with integral maps
***************/
__FILE_BUFFERED__
__MEMORY_BUFFERED__
static int generate_all_feature_values_and_clssfid(u32* integ1, u32* integ2, int w, int h)
{
    int t, s;
    int sw = sample_width;
    int sh = sample_width;

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

    u8 _t_arr[__temp_kinds] = T_ARR;
    u8 _s_arr[__temp_kinds] = S_ARR;
    u8 _tt_arr[__temp_kinds] = TT_ARR;

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
                        int *fv = NULL;
                        int *fv_up = 0;
                        int *fv_down = 0;

                        if((j % t) != 0 || (i % s) != 0)
                        {
                            printf("[ERROR]  j or i wrong! %d\n", j, i);
                        }
                        int siz1 = _p_fv_creater_arr[temp](integ1, w, h, sw, sh, x, y, j, i, &fv_up);
                        int siz2 = _p_fv_creater_arr[temp](integ2, w, h, sw, sh, x, y, j, i, &fv_down);
                        int siz = siz1 + siz2;

                        fv = (int*) malloc(siz * sizeof(int));

                        memcpy(fv, fv_up, siz1 * sizeof(int));
                        memcpy(fv + siz1, fv_down, siz2 * sizeof(int));

                        write_file_by_id(FEATURE_PATH, x, y, j, i,
                                        _tt_arr[temp], 0, (char*)fv, siz * sizeof(int));

                        struct __clssf *cls = create_clssfier(x, y, j, i, _tt_arr[temp], 0, siz);
                        g_classifiers.push_back(cls);

                        free(fv);
                        free(fv_up);
                        free(fv_down);
                    }
                }
            }
        }
    }
    save_clssf_index();
}

/*********
* (4) Do limits numbers' iteration, get all Weak Classifiers
**************/
__FILE_BUFFERED__
static int do_iterator_weak_clssfier(int begin_from)
{

    for(int rt = begin_from; rt < TRAINING_TIMES; rt ++) {

        printf("the %d begins...\n", rt);

        struct __clssf* wkcls = create_next_weak_clss(g_weights, g_loss, max_number_samples);

        char file[512] = {0};
        sprintf(file, "%s/wkclss_%d", WEAK_CLASSFIER_PATH, rt);
        save_weak_clss_to_file(file, wkcls);

        recalculate_weights(wkcls, g_weights, g_loss);

        //for(int i = 0; i < max_number_samples; i ++) {
         //   printf("%f ", g_weights[i]);
        //}


        memset(file, 0, 512);
        sprintf(file, "%s/weights_%d", WEIGHT_LOSS_PATH, rt);
        write_file(file, (char*)g_weights, max_number_samples * sizeof(double));

        memset(file, 0, 512);
        sprintf(file, "%s/loss_%d", WEIGHT_LOSS_PATH, rt);
        write_file(file, (char*)g_loss, max_number_samples * sizeof(int));

        //free(wkcls);

        printf("the %d weak classifier created [%f  %f]  %d [%d%d]\n", rt, wkcls->am, wkcls->em, wkcls->thrhd, wkcls->tt, wkcls->tn);

        if(wkcls->em > 0.5f)
        {
            break;
        }
        //free(wkcls);
        g_strocls->vwkcs.push_back(wkcls);
        g_strocls->nwk ++;
    }
    save_strong_clss_to_file(g_strocls);

    /// do not need to free all (wkcls)

    return 0;
}

int adaboost_train(u8* sample1, u8* sample2, int w, int h, int iter_from)
{
    u32* integral1 = NULL;
    u32* integral2 = NULL;

    ///1
    init();

    ///2
    load_samples_get_integral(sample1, sample2, &integral1, &integral2, w, h);

    ///3
    generate_all_feature_values_and_clssfid(integral1, integral2, w, h);

    //load_clssf_index();

    ///4
    do_iterator_weak_clssfier(iter_from);
}
