#include <iostream>
#include "proc.h"
#include "cv.h"
#include "highgui.h"

using namespace std;

extern BinaryProc g_binary;

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

        g_binary.process((u8*)p_frame->imageData, buf, p_frame->width, p_frame->height, 125);

        memcpy(p_frame->imageData, buf, p_frame->width * p_frame->height * p_frame->nChannels);

        cvShowImage("video", p_frame);

        free(buf);

        char c = cvWaitKey(33);
        if(c == 27) break;
    }



    cvReleaseCapture(&p_capture);
    cvDestroyWindow("video");

    return 0;
}
