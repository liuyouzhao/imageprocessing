
#include <stdio.h>
#include <stdlib.h>
#include "proc.h"
#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

extern int adaboost_merge_samples(const char* path, int file_num,
                                    int nw, int nh, int sw, int sh, u8 **merged);
extern int adaboost_train(u8* sample1, u8* sample2, int w, int h, int bf = 0);
extern int adaboost_train(int bf = 0);

extern int adaboost_face_test(u8 *image, u8 *origin, int w, int h);
extern GreyProc g_grey;
extern std::vector<struct __possi_rect*> g_possi_rects;
extern struct __possi_rect** faces_value;

void show_image(u8* buffer, int w, int h);

/**
#define SAMPLE_BLOCK_WIDTH 20
#define SAMPLE_NUMBER 2000
#define SAMPLE_MERGED_WIDTH 50
#define SAMPLE_MERGED_HEIGHT 40
*/
int adaboost_train_main() {

#if 1
    u8 *merged1 = 0;
    u8 *merged2 = 0;
    adaboost_merge_samples(SAMPLE1_PATH,
                            SAMPLE_NUMBER,
                            SAMPLE_MERGED_WIDTH,
                            SAMPLE_MERGED_HEIGHT,
                            SAMPLE_BLOCK_WIDTH,
                            SAMPLE_BLOCK_WIDTH, &merged1);

    adaboost_merge_samples(SAMPLE2_PATH,
                            SAMPLE_NUMBER,
                            SAMPLE_MERGED_WIDTH,
                            SAMPLE_MERGED_HEIGHT,
                            SAMPLE_BLOCK_WIDTH,
                            SAMPLE_BLOCK_WIDTH, &merged2);

    show_image(merged1, SAMPLE_MERGED_WIDTH*SAMPLE_BLOCK_WIDTH, SAMPLE_MERGED_HEIGHT*SAMPLE_BLOCK_WIDTH);
    show_image(merged2, SAMPLE_MERGED_WIDTH*SAMPLE_BLOCK_WIDTH, SAMPLE_MERGED_HEIGHT*SAMPLE_BLOCK_WIDTH);

    adaboost_train(merged1, merged2, SAMPLE_MERGED_WIDTH*SAMPLE_BLOCK_WIDTH,
                            SAMPLE_MERGED_HEIGHT*SAMPLE_BLOCK_WIDTH, 0);
#else
    adaboost_train();
#endif

    return 0;
}
#define WHOLE_PICTURE_TEST 1
int adaboost_single_face_judge(u8 *image, int block);
int adaboost_test_main() {

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

    int block_size = 200;
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

    adaboost_face_test(image, image, screen_width, screen_height);



    memcpy(p_frame->imageData, image, p_frame->width * p_frame->height * p_frame->nChannels);

    //std::vector<struct __possi_rect*>::iterator it;
    //for(it = g_possi_rects.begin(); it < g_possi_rects.end(); it ++) {
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
