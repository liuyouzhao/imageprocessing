#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include "proc.h"
#include "proc_utils.h"

#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

#define WHOLE_PICTURE_TEST 1

struct __possi_rect** faces_value = 0;
int load_strong_clss_from_file(const char* file, __strong_clssf** stroclssf);
extern HaarlikeProc g_haarlike;

int adaboost_single_face_judge(u8 *image, int block);
static int get_rect_face_fv( u32* integral,
                             int w, int h,
                             int wx, int wy,
                             int wblock,
                             int x, int y,
                             int block_width, int block_height,
                             int template_type, int template_index,
                             int sample_block );


static int adaboost_jump_block_face(u32* integral, int w, int h,
                                    int start_x, int start_y, int window, __strong_clssf* pstr_clss)
{
    double score = 0.0f;
    double scoreav = 0.0f;
    int yes = 0;
    int all = 0;

    std::vector<struct __clssf*>::iterator it;
    for( it = pstr_clss->vwkcs.begin(); it != pstr_clss->vwkcs.end(); it ++ ) {
        struct __clssf* p = *it;
        int fv = get_rect_face_fv(integral, w, h,
                                    start_x, start_y, window,
                                    p->x, p->y, p->bw, p->bh, p->tt, p->tn, SAMPLE_BLOCK_WIDTH);

#if 0
        int rt = fv >= p->thrhd ? 1 : 0;
        score += (double)rt * (p->am);
        scoreav += 0.5f * p->am;
#else
        all ++;
        if(p->tn == 0)
        {
            int rt = fv >= p->thrhd ? 1 : 0;
            score += (double)rt * (p->am);
            scoreav += 0.5f * p->am;
            if(fv >= p->thrhd) yes ++;
        }
        else
        {
            int rt = fv < p->thrhd ? 1 : 0;
            score += (double)rt * (p->am);
            scoreav += 0.5f * p->am;
            if(fv < p->thrhd) yes ++;
        }
#endif

        //printf("%d, %f     %f [%d  %d]\n", rt, score, scoreav, p->thrhd, fv);
    }
    //printf("score: %f        scoreav: %f  %d %d\n",  score, scoreav, yes, all);
    if(score > scoreav) {
        return (int)(10000 * (score - scoreav));
    }
    return 0;
}

static int get_rect_face_fv( u32* integral,
                             int w, int h,
                             int wx, int wy,
                             int wblock,
                             int x, int y,
                             int block_width, int block_height,
                             int template_type, int template_index,
                             int sample_block )
{

    int _x = 0;
    int _y = 0;
    int _bw = 0;
    int _bh = 0;
    _x = (int)((double)x * ((double)wblock / (double)sample_block));
    _y = (int)((double)y * ((double)wblock / (double)sample_block));
    _x += wx;
    _y += wy;

    _bw = block_width * ((double)wblock / (double)sample_block);
    _bh = block_height * ((double)wblock / (double)sample_block);
    int *ftout = 0;
    int fv = 0;

    int (*_p_fv_creater_arr[5]) (
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

    _p_fv_creater_arr[template_type](integral, w, h, w, h, _x, _y, _bw, _bh, &ftout);
    fv = (double)(ftout[0]) / ((double)wblock / (double)sample_block);
    //fv = ftout[0];
    free(ftout);
    return fv;
}

int adaboost_face_test(IplImage* p_frame, u8 *image, int w, int h)
{
    u32* integral = (u32*) malloc(w * h * sizeof(int));

    struct __strong_clssf* _str_clssf;

    load_strong_clss_from_file(STRONG_CLASSFIER_FILE, &_str_clssf);

    g_haarlike.haarlike_integral(image, integral, w, h);

    int block = 20;
    int max_block = 26;
    for(; block < max_block; block += 2)
    {
        for(int i = 0; i < h - block; i += 2)
        {
            for(int j = 0; j < w - block; j += 2)
            {
                int rt = adaboost_jump_block_face(integral, w, h, j, i, block, _str_clssf);
                //printf("%d ----\n", rt);
                if(rt)
                    cvRectangle(p_frame, cvPoint(j, i), cvPoint(j + block, i + block), CV_RGB(255,255,255), 2);
            }
        }

    }

}

int adaboost_test_main()
{

    IplImage* p_frame = NULL;

#if USE_CAMERA
    int screen_width = 640;
    int screen_height = 480;

    int block_size = 200;
    CvCapture* p_capture = cvCreateCameraCapture(0);

    cvSetCaptureProperty(p_capture, CV_CAP_PROP_FRAME_WIDTH, screen_width);
    cvSetCaptureProperty(p_capture, CV_CAP_PROP_FRAME_HEIGHT, screen_height);

    while(1)
    {
        p_frame = cvQueryFrame(p_capture);

        int width = p_frame->width;
        int height = p_frame->height;

        u8 *buf = (u8*)malloc(screen_width * screen_height);
        u8 *buf_target = (u8*)malloc(block_size * block_size);

        memset(buf,screen_width * screen_height, 0);

        g_grey.process((u8*)p_frame->imageData, buf, screen_width, screen_height, 1);

/*
        for(int i = 0, i2 = (screen_height >> 1) - (block_size >> 1); i < block_size; i ++, i2 ++) {
            for(int j = 0, j2 = (screen_width >> 1) - (block_size >> 1); j < block_size; j ++, j2 ++) {
                buf_target[i * block_size + j] = buf[i2 * screen_width + j2];
            }
        }

        for(int i = 0, i2 = (screen_height >> 1) - (block_size >> 1); i < block_size; i ++, i2 ++) {
            for(int j = 0, j2 = (screen_width >> 1) - (block_size >> 1); j < block_size; j ++, j2 ++) {
                buf[i2 * screen_width + j2] = buf_target[i * block_size + j];
            }
        }
*/

       // double score1 = adaboost_face_test(STRONG_CLASSFIER_FILE1, buf_target, block_size);
        int isface = adaboost_face_test(buf, (u8*)p_frame->imageData, screen_width, screen_height);

        //printf("%d\n", isface);

        //memcpy(p_frame_grey->imageData, buf, screen_width * screen_height);
        cvShowImage("video", p_frame);

        char c = cvWaitKey(33);
        if(c == 27) break;
    }
#elif WHOLE_PICTURE_TEST
    int screen_width = 640;
    int screen_height = 480;

    int target_width = 320;
    int target_height = 240;
    int b = 2;

    int block_size = 200;
    cv::Mat src;
    src = cv::imread(TEST_FILE);

    p_frame = cvCreateImage(cvSize(target_width, target_height), IPL_DEPTH_8U, 1);

    if(!src.data)
    {
        printf("Error! file is empty! \n");
        return -1;
    }

    int nr = src.rows;
    int nc = src.cols;
    int c = src.channels();
    u8 *image = (u8*)malloc(nr * nc * c);
    u8 *image2 = (u8*)malloc(target_height * target_width * c);

    int n = 0;
    for(int i = 0; i < nr; i ++) {
        uchar* data = src.ptr<uchar>(i);
        for(int j = 0; j < nc; j ++) {
            image[n] = data[j * c];
            n ++;
        }
    }

    for(int i = 0, i2 = 0; i2 < target_height; i += b, i2 ++)
    {
        for(int j = 0, j2 = 0; j2 < target_width; j += b, j2 ++)
        {
            image2[i2 * target_width + j2] = image[i * screen_width + j];
        }
    }
    //u8 *buf = (u8*)malloc(screen_width * screen_height);
    //memset(buf, screen_width * screen_height, 0);

    //g_grey.process(image, buf, screen_width, screen_height, 1);

    //adaboost_face_test(image, image, screen_width, screen_height);
    memcpy(p_frame->imageData, image2, p_frame->width * p_frame->height * p_frame->nChannels);

    adaboost_face_test(p_frame, image2, target_width, target_height);


    //memcpy(p_frame->imageData, image, p_frame->width * p_frame->height * p_frame->nChannels);

    //std::vector<struct __possi_rect*>::iterator it;
    //for(it = g_possi_rects.begin(); it < g_possi_rects.end(); it ++) {
#if 0
    int drawed = 0;
    for(int i = 1023; i >= 0 && drawed < 10; i --) {

        struct __possi_rect* p = faces_value[i];
        if(!p || p->rt == 0) continue;


        //cvCircle(p_frame, cvPoint(p->x + p->w / 2, p->y + p->h / 2), (int)(p->w)/2, CV_RGB(0,255,0), 2);

        //cvCircle(p_frame, cvPoint(p->x + p->w / 2, p->y + p->h / 2), 5, CV_RGB(0,255,0), 2);
        drawed ++;
        cvRectangle(p_frame, cvPoint(p->x, p->y), cvPoint(p->x + p->w, p->y + p->h), CV_RGB(0,255,0), 2);
        //cvRect(p_frame, p->x, p->y, p->w, p->h);
    }
#endif

    while(1) {
        cvShowImage("show", p_frame);
        char k = cvWaitKey(33);
    }
#else
#if 0
    int screen_width = 20;
    int screen_height = 20;

    cv::Mat src;
    src = cv::imread(TEST_FILE);

    p_frame = cvCreateImage(cvSize(screen_width, screen_height), IPL_DEPTH_8U, 1);

    if(!src.data)
    {
        printf("Error! file is empty! \n");
        return -1;
    }

    int nr = src.rows;
    int nc = src.cols;
    int c = src.channels();
    printf("c %d \n", c);

    u8 *image = (u8*)malloc(nr * nc * c);

    int n = 0;
    for(int i = 0; i < nr; i ++) {
        uchar* data = src.ptr<uchar>(i);
        for(int j = 0; j < nc; j ++) {
            image[n] = data[j * c];
            n ++;
        }
    }
    //u8 *buf = (u8*)malloc(screen_width * screen_height);
    //memset(buf, screen_width * screen_height, 0);

    //g_grey.process(image, buf, screen_width, screen_height, 1);

    //adaboost_face_test(image, image, screen_width, screen_height);
    memcpy(p_frame->imageData, image, p_frame->width * p_frame->height * p_frame->nChannels);

    int ret = adaboost_single_face_judge(image, screen_width);
    printf("ret: %d\n", ret);
    while(1) {
        cvShowImage("show", p_frame);
        char k = cvWaitKey(33);
    }
#endif

    int screen_width = 20;
    int screen_height = 20;
    u8 *image = 0;
    int num_success = 0;
    for(int num = 2001; num <= 2700; num ++) {

        char file[256] = {};
        sprintf(file, "%s/%d.bmp", SAMPLE1_PATH, num);

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

        if(image == 0)
            image = (u8*)malloc(nr * nc);

        int n = 0;
        for(int i = 0; i < nr; i ++) {
            uchar* data = src.ptr<uchar>(i);
            for(int j = 0; j < nc; j ++) {
                image[n] = data[j * c];
                n ++;
            }
        }
        int ret = adaboost_single_face_judge(image, screen_width);

        num_success = ret ? num_success + 1 : num_success;
        printf("ret: %d\n", ret);
    }
    printf("2000 : %d\n", num_success);

#endif


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
