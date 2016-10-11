
#include <stdio.h>
#include <stdlib.h>
#include "proc.h"
#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

extern int adaboost_merge_samples(const char* path, int file_num,
                                    int nw, int nh, int sw, int sh, u8 **merged);
extern int adaboost_train(u8* sample1, u8* sample2, int w, int h, int sw, int sh);

extern int adaboost_face_test(u8 *image, int image_block);
extern GreyProc g_grey;

/**
#define SAMPLE_BLOCK_WIDTH 20
#define SAMPLE_NUMBER 2000
#define SAMPLE_MERGED_WIDTH 50
#define SAMPLE_MERGED_HEIGHT 40
*/
int adaboost_train_main() {

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

    adaboost_train(merged1, merged2, SAMPLE_MERGED_WIDTH*SAMPLE_BLOCK_WIDTH,
                            SAMPLE_MERGED_HEIGHT*SAMPLE_BLOCK_WIDTH,
                            SAMPLE_BLOCK_WIDTH, SAMPLE_BLOCK_WIDTH);

    return 0;
}

int adaboost_test_main() {

    int screen_width = 320;
    int screen_height = 240;

    int block_size = 200;

    CvCapture* p_capture = cvCreateCameraCapture(1);

    cvSetCaptureProperty(p_capture, CV_CAP_PROP_FRAME_WIDTH, screen_width);
    cvSetCaptureProperty(p_capture, CV_CAP_PROP_FRAME_HEIGHT, screen_height);

    IplImage* p_frame = NULL;
    IplImage* p_frame_grey = cvCreateImage(cvSize(screen_width, screen_height), IPL_DEPTH_8U, 1);

    cvNamedWindow("video", 1);

    while(1)
    {
        p_frame = cvQueryFrame(p_capture);

        int width = p_frame->width;
        int height = p_frame->height;

        u8 *buf = (u8*)malloc(screen_width * screen_height);
        u8 *buf_target = (u8*)malloc(block_size * block_size);

        memset(buf,screen_width * screen_height, 0);

        g_grey.process((u8*)p_frame->imageData, buf, screen_width, screen_height, 1);


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

       // double score1 = adaboost_face_test(STRONG_CLASSFIER_FILE1, buf_target, block_size);
        int isface = adaboost_face_test(buf_target, block_size);

        printf("%d\n", isface);

        memcpy(p_frame_grey->imageData, buf, screen_width * screen_height);
        cvShowImage("video", p_frame_grey);

        char c = cvWaitKey(33);
        if(c == 27) break;
    }
}
