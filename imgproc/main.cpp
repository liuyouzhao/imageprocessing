#include <iostream>
#include "proc.h"
#include "cv.h"
#include "highgui.h"

using namespace std;

extern GreyProc g_grey;
extern BinaryProc g_binary;
extern LoneedgeProc g_longedge;
extern GradientProc g_gradient;
extern GaussProc g_gauss;

int main()
{

    IplImage* p_frame = NULL;

    CvCapture* p_capture = cvCreateCameraCapture(-1);

    cvNamedWindow("video", 1);

    while(1) {
        p_frame = cvQueryFrame(p_capture);

        if(!p_frame) {
            break;
        }

        u8 *buf = (u8*)malloc(p_frame->width * p_frame->height * p_frame->nChannels);
        memset(buf, p_frame->width * p_frame->height * p_frame->nChannels, 0);

        g_grey.process((u8*)p_frame->imageData, buf, p_frame->width, p_frame->height);
        memcpy(p_frame->imageData, buf, p_frame->width * p_frame->height * p_frame->nChannels);
        memset(buf, p_frame->width * p_frame->height * p_frame->nChannels, 0);

        g_gauss.process((u8*)p_frame->imageData, buf, p_frame->width, p_frame->height, 5);
        memcpy(p_frame->imageData, buf, p_frame->width * p_frame->height * p_frame->nChannels);
        memset(buf, p_frame->width * p_frame->height * p_frame->nChannels, 0);

        g_gradient.process((u8*)p_frame->imageData, buf, p_frame->width, p_frame->height);
        memcpy(p_frame->imageData, buf, p_frame->width * p_frame->height * p_frame->nChannels);
        memset(buf, p_frame->width * p_frame->height * p_frame->nChannels, 0);

        g_binary.process((u8*)p_frame->imageData, buf, p_frame->width, p_frame->height, 30);
        memcpy(p_frame->imageData, buf, p_frame->width * p_frame->height * p_frame->nChannels);
        memset(buf, p_frame->width * p_frame->height * p_frame->nChannels, 0);
#if 1

        g_longedge.process((u8*)p_frame->imageData, buf, p_frame->width, p_frame->height, 30);
        memcpy(p_frame->imageData, buf, p_frame->width * p_frame->height * p_frame->nChannels);
        memset(buf, p_frame->width * p_frame->height * p_frame->nChannels, 0);
#endif
        cvShowImage("video", p_frame);

        free(buf);

        char c = cvWaitKey(33);
        if(c == 27) break;
    }



    cvReleaseCapture(&p_capture);
    cvDestroyWindow("video");

    return 0;
}