#include <stdio.h>

#include "proc.h"
#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

#define TEST_FILE_SURF "/home/hujia/workspace/imgproc/imageprocessing/imgproc/res/tests/teste.jpg"
#define TEST_FILE_SURF2 "/home/hujia/workspace/imgproc/imageprocessing/imgproc/res/tests/teste2.jpg"
#define PAIR_THREHOLD 0

class SurfObject;

static SurfObject* s_old;
static SurfObject* s_new;
extern GaussProc g_gauss;

void load_grey_image_from_file(const char* file, u8** image, int *w, int *h);
void rotate_point(double centerX, double centerY, double angle, double* x, double* y);

std::vector<struct __surf_match_pair*> vec_match_pair;
struct __surf_match_pair pairs1[512] = {0};
struct __surf_match_pair pairs2[512] = {0};
struct __surf_match_pair pairs_final[512] = {0};
int __pair_num1 = 0;
int __pair_num2 = 0;
int __pair_num_final = 0;

int debug_show_feature(SurfObject* surf, IplImage* frame, int w, int h)
{

#if 0
    double x1 = w/2 - 40.0f;
    double y1 = h/2 - 40.0f;
    double x2 = w/2 + 40.0f;
    double y2 = h/2 - 40.0f;
    double x3 = w/2 + 40.0f;
    double y3 = h/2 + 40.0f;
    double x4 = w/2 - 40.0f;
    double y4 = h/2 + 40.0f;
    rotate_point((double)w/2, (double)h/2, 95.0f/360.0f*2*PI, &x1, &y1);
    rotate_point((double)w/2, (double)h/2, 95.0f/360.0f*2*PI, &x2, &y2);
    rotate_point((double)w/2, (double)h/2, 95.0f/360.0f*2*PI, &x3, &y3);
    rotate_point((double)w/2, (double)h/2, 95.0f/360.0f*2*PI, &x4, &y4);
    cvLine(frame, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(255,255,255), 5);
    cvLine(frame, cvPoint(x2, y2), cvPoint(x3, y3), CV_RGB(255,255,255), 5);
    cvLine(frame, cvPoint(x3, y3), cvPoint(x4, y4), CV_RGB(255,255,255), 5);
    cvLine(frame, cvPoint(x4, y4), cvPoint(x1, y1), CV_RGB(255,255,255), 5);

    show_image((u8*)(frame->imageData), w, h);
    return 0;
#endif
    std::vector<struct __hessian_fv*> vec = surf->getFeatureValues();
    std::vector<struct __hessian_fv*>::iterator it = vec.begin();
    for(; it != vec.end(); it ++)
    {
        struct __hessian_fv* fv = *it;
        int x = fv->x;
        int y = fv->y;
        int dir = fv->dir;
        double angle = (double)dir / 360.0f * 2 * 3.1415926;
        double s = fv->s;
        int bw = s * HESSIAN_DESCRIPT_BLOCK / 2;
        bw = bw >> 1;
        double x1 = x - bw;
        double y1 = y - bw;
        double x2 = x + bw;
        double y2 = y - bw;
        double x3 = x + bw;
        double y3 = y + bw;
        double x4 = x - bw;
        double y4 = y + bw;
        double xd = x + bw;
        double yd = y;

        //cvCircle(frame, cvPoint(x, y), angle * angle, CV_RGB(0,255,0), 2);
#if 1
        rotate_point(x, y, angle, &x1, &y1);
        rotate_point(x, y, angle, &x2, &y2);
        rotate_point(x, y, angle, &x3, &y3);
        rotate_point(x, y, angle, &x4, &y4);
        rotate_point(x, y, angle, &xd, &yd);

        cvLine(frame, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(255,255,0), 1);
        cvLine(frame, cvPoint(x2, y2), cvPoint(x3, y3), CV_RGB(255,255,0), 1);
        cvLine(frame, cvPoint(x3, y3), cvPoint(x4, y4), CV_RGB(255,255,0), 1);
        cvLine(frame, cvPoint(x4, y4), cvPoint(x1, y1), CV_RGB(255,255,0), 1);

        cvLine(frame, cvPoint(x, y), cvPoint(xd, yd), CV_RGB(255,255,0), 2);
#endif
        //printf("%p %f\n", surf, angle);
        //cvRectangle(frame, cvPoint(x - fv->level * 4, y - fv->level * 4),
        //                cvPoint(x + fv->level * 4, y + fv->level * 4), CV_RGB(125,125,125), 1);
    }



    return 0;
}

void debug_show_pair(IplImage* left, IplImage* right, int w1, int h1, int w2, int h2)
{
    int w = w1 + w2;
    int h = h1 > h2 ? h1 : h2;
    IplImage* image = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);

    for(int i = 0; i < h1; i ++)
    {
        memcpy(image->imageData + i * w*3, left->imageData + i * w1*3, w1 * 3);
    }

    for(int i = 0; i < h2; i ++)
    {
        memcpy(image->imageData + i * w*3 + w1*3, right->imageData + i * w2*3, w2 * 3);
    }


    //std::vector<struct __surf_match_pair*>::iterator itp = vec_match_pair.begin();
    //for(; itp != vec_match_pair.end(); itp ++)
    for(int i = 0; i < __pair_num_final; i ++)
    {
        struct __surf_match_pair* pr = &pairs_final[i];
        cvCircle(image, cvPoint(pr->x1, pr->y1), 5, CV_RGB(255, 0, 0));
        cvCircle(image, cvPoint(pr->x2 + w1, pr->y2), 5, CV_RGB(255, 0, 0));
        cvLine(image, cvPoint(pr->x1, pr->y1), cvPoint(pr->x2 + w1, pr->y2), CV_RGB(255, 255, 0));
    }
    //printf("%d\n", __pair_num_final);


    cvShowImage("show", image);

    cvReleaseImage(&image);

}

int process_go(SurfObject* s_old, u8* image, int w, int h)
{
     __perf_begin_time();
    /// (0) update image
    s_old->updateImage(image, w, h);

    __perf_end_time();
    __perf_begin_time();
    /// (1) update integral
    s_old->updateCurrentIntegral();
    __perf_end_time();


    __perf_begin_time();
    /// (2) get all parimits features
    s_old->updateAllParimitsFeatures();
    __perf_end_time();


    __perf_begin_time();
    /// (3) update all extremems
    s_old->updateCalculateExtremums();
    __perf_end_time();


    __perf_begin_time();
    /// (4) update directions
    s_old->updateCalculateDirections();
    __perf_end_time();

    //debug_show_feature(s_old, pic, w, h);

    //debug_show_feature(s_old, p_frame, w, h);
    __perf_begin_time();
    /// (5) update description
    s_old->updateFeatureDescription();
    __perf_end_time();
}

void clear_pair()
{
    std::vector<struct __surf_match_pair*>::iterator it = vec_match_pair.begin();
    for(; it != vec_match_pair.end(); it ++)
    {
        struct __surf_match_pair* p = *it;
        free(p);
    }
    vec_match_pair.clear();
    memset(pairs1 , 0, sizeof(struct __surf_match_pair) * 512);
    memset(pairs2 , 0, sizeof(struct __surf_match_pair) * 512);
    __pair_num1 = 0;
    __pair_num2 = 0;
}

void clear_single_pair(struct __surf_match_pair* pairs, int* __num)
{
    memset(pairs, 0, sizeof(struct __surf_match_pair) * 512);
    *__num = 0;
}


void match_pair(SurfObject* left, SurfObject* right, struct __surf_match_pair* pairs, int* num)
{
    clear_single_pair(pairs, num);

    //std::vector<struct __hessian_fv*>::iterator itfv = left->getFeatureValues().begin();
    //for(; itfv != left->getFeatureValues().end(); itfv ++)
    for(int i = 0; i < left->m_num_fvs; i ++)
    {
        struct __hessian_fv* fv = &(left->m_fvs[i]);

        double maxv = -99999;
        double minv = 999999;
        int x1 = 0;
        int y1 = 0;
        int x2 = 0;
        int y2 = 0;
        double* v1 = fv->fvs;
        //printf("%p \n", v1);

        //std::vector<struct __hessian_fv*>::iterator itfv2 = right->getFeatureValues().begin();
        //for(; itfv2 != right->getFeatureValues().end(); itfv2 ++)
        for(int j = 0; j < right->m_num_fvs; j ++)
        {

            struct __hessian_fv* fv2 = &(right->m_fvs[j]);

            double* v2 = fv2->fvs;
            double curv = 0;


            for(int p = 0; p < 64; p ++)
            {
                curv += (v1[p]-v2[p])*(v1[p]-v2[p]);
                //printf("%f %f ", v1[p], v2[p]);
            }
            curv = sqrt(curv);

            //printf("%f ", curv);

            if(curv < minv)
            {
                minv = curv;
                x1 = fv->x;
                y1 = fv->y;
                x2 = fv2->x;
                y2 = fv2->y;
            }
        }
        struct __surf_match_pair* pr = &pairs[*num];
        pr->x1 = x1;
        pr->x2 = x2;
        pr->y1 = y1;
        pr->y2 = y2;
        pr->distance = minv;

        (*num) ++;
            //vec_match_pair.push_back(pr);

    }
}

int duplicate_pairs(struct __surf_match_pair* pairs1, int num1,
                    struct __surf_match_pair* pairs2, int num2,
                    struct __surf_match_pair* pair_final, int* num_final)
{
    clear_single_pair(pair_final, num_final);
    for(int i = 0; i < num1; i ++)
    {
        int x1 = pairs1[i].x1;
        int y1 = pairs1[i].y1;
        int x2 = pairs1[i].x2;
        int y2 = pairs1[i].y2;

        for(int j = 0; j < num2; j ++)
        {
            if(x1 == pairs2[j].x2 && x2 == pairs2[j].x1 &&
                y1 == pairs2[j].y2 && y2 == pairs2[j].y1)
            {
                pair_final[*num_final].x1 = x1;
                pair_final[*num_final].y1 = y1;
                pair_final[*num_final].x2 = x2;
                pair_final[*num_final].y2 = y2;
                (*num_final) ++;
            }
        }
    }
}

int surf_main()
{
#if 1
    int w = 320;
    int h = 240;
    CvCapture* p_capture = cvCreateCameraCapture(1);

    cvSetCaptureProperty(p_capture, CV_CAP_PROP_FRAME_WIDTH, w);
    cvSetCaptureProperty(p_capture, CV_CAP_PROP_FRAME_HEIGHT, h);

    u8* left = 0;
    u8* left_grey = 0;
    u8* right_grey = 0;

    IplImage* p_frame = NULL;
    IplImage* p_show1 = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);

    s_old = new SurfObject();
    s_new = new SurfObject();

    left_grey = (u8*)malloc(w * h);
    right_grey = (u8*)malloc(w * h);

    int time = 0;
    while(1)
    {
        p_frame = cvQueryFrame(p_capture);

        if(!p_frame)
        {
            break;
        }

        char k = cvWaitKey(33);
        if(k == 27) break;

        if(k == 32)
        {
            if(time == 0)
            {
                if(left == 0)
                {
                    left = (u8*) malloc(w * h * 3);
                }
                memcpy(left, p_frame->imageData, w * h * 3);
                memcpy(p_show1->imageData, p_frame->imageData, w * h * 3);

                rgb2grey(left, left_grey, w, h);
                process_go(s_old, left_grey, w, h);
                time = 1;
            }
            else if(time == 1)
            {
                time = 0;

            }
        }

        if(left)
        {
            rgb2grey((u8*) p_frame->imageData, right_grey, w, h);


            process_go(s_new, right_grey, w, h);

            //debug_show_feature(s_old, p_show1, w, h);
            //debug_show_feature(s_new, p_frame, w, h);
#if 1
            match_pair(s_old, s_new, pairs1, &__pair_num1);
            match_pair(s_new, s_old, pairs2, &__pair_num2);
            duplicate_pairs(pairs1, __pair_num1, pairs2, __pair_num2, pairs_final, &__pair_num_final);
#else
            match_pair(s_old, s_new, pairs_final, &__pair_num_final);
#endif
            get_matrix_by_points(pairs_final, __pair_num_final);
            debug_show_pair(p_show1, p_frame, w, h, w, h);

            delete s_new;
            s_new = new SurfObject();
        }
        if(!left)
        {
            cvShowImage("show", p_frame);
        }


    }

#endif // 0

#if 0
    IplImage* p_frame = NULL;


    u8* image = 0;
    u8* image2 = 0;
    //u8* image_color = 0;

    int w = 0;
    int h = 0;

    int w2 = 0;
    int h2 = 0;

    //load_color_image_from_file(TEST_FILE_SURF, &image_color, &w, &h);
    load_grey_image_from_file(TEST_FILE_SURF, &image, &w, &h);
    load_grey_image_from_file(TEST_FILE_SURF2, &image2, &w2, &h2);

    //show_image(image, w, h);
    //show_image(image2, w2, h2);

    IplImage* pic = cvLoadImage(TEST_FILE_SURF);
    IplImage* pic2 = cvLoadImage(TEST_FILE_SURF2);


    s_old = new SurfObject();
    s_new = new SurfObject();

    process_go(s_old, image, w, h);
    process_go(s_new, image2, w2, h2);


    debug_show_feature(s_old, pic, w, h);
    debug_show_feature(s_new, pic2, w2, h2);
#if 1
    while(1) {
        cvShowImage("show", pic);
        cvShowImage("show2", pic2);
        char k = cvWaitKey(33);
        if(k == 27) break;
    }
#endif
    return 0;
#if 1
    match_pair(s_old, s_new, pairs1, &__pair_num1);
    match_pair(s_new, s_old, pairs2, &__pair_num2);
    duplicate_pairs(pairs1, __pair_num1, pairs2, __pair_num2, pairs_final, &__pair_num_final);
#else
    match_pair(s_old, s_new, pairs_final, &__pair_num_final);
#endif
    while(1)
    {
        debug_show_pair(pic, pic2, w, h, w2, h2);
        char k = cvWaitKey(33);
        if(k == 27) break;
    }


#endif
    return 0;
}
