#include <stdio.h>

#include "proc.h"
#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

class SurfObject;

static SurfObject* s_old;
static SurfObject* s_new;
extern GaussProc g_gauss;

void load_grey_image_from_file(const char* file, u8** image, int *w, int *h);
void rotate_point(double centerX, double centerY, double angle, double* x, double* y);

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


        rotate_point(x, y, angle, &x1, &y1);
        rotate_point(x, y, angle, &x2, &y2);
        rotate_point(x, y, angle, &x3, &y3);
        rotate_point(x, y, angle, &x4, &y4);

        cvLine(frame, cvPoint(x1, y1), cvPoint(x2, y2), CV_RGB(255,0,0), 1);
        cvLine(frame, cvPoint(x2, y2), cvPoint(x3, y3), CV_RGB(255,0,0), 1);
        cvLine(frame, cvPoint(x3, y3), cvPoint(x4, y4), CV_RGB(255,0,0), 1);
        cvLine(frame, cvPoint(x4, y4), cvPoint(x1, y1), CV_RGB(255,0,0), 1);
        //cvRectangle(frame, cvPoint(x - fv->level * 4, y - fv->level * 4),
        //                cvPoint(x + fv->level * 4, y + fv->level * 4), CV_RGB(125,125,125), 1);
    }
    while(1) {
        cvShowImage("show", frame);
        char k = cvWaitKey(33);
        if(k == 27) break;
    }
    return 0;
}

int surf_main()
{
    IplImage* p_frame = NULL;


    u8* image = 0;
    //u8* image_color = 0;

    int w = 0;
    int h = 0;

    //load_color_image_from_file(TEST_FILE_SURF, &image_color, &w, &h);
    load_grey_image_from_file(TEST_FILE_SURF, &image, &w, &h);
    IplImage* pic = cvLoadImage(TEST_FILE_SURF);



    p_frame = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);

    memcpy(p_frame->imageData, image, w * h * p_frame->nChannels);

    s_old = new SurfObject();

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

    debug_show_feature(s_old, pic, w, h);

    //debug_show_feature(s_old, p_frame, w, h);
    __perf_begin_time();
    /// (5) update description
    s_old->updateFeatureDescription();
    __perf_end_time();

    return 0;
}
