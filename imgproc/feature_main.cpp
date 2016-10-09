#include <stdio.h>
#include <stdlib.h>
#include "proc.h"
#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

using namespace std;

extern HaarlikeProc g_haarlike;

int write_file(const char* file, int *data, int len) {
    FILE* fd = fopen(file, "w");
    int nblock = fwrite(data, len, 1, fd);

    fflush(fd);
    fclose(fd);

    printf("%s\n", file);
}

int feature_main(const char* file)
{
    cv::Mat src;
    src = cv::imread(file);

    if(!src.data)
    {
        printf("No data!--Exiting the program \n");
        return -1;
    }

    int nr = src.rows;
    int nc = src.cols;
    int c = src.channels();

    u8 *gm = (u8*) malloc(nr * nc);

    for(int i = 0; i < nr; i ++) {
        uchar* data = src.ptr<uchar>(i);
        for(int j = 0; j < nc; j ++) {
            gm[i * nc + j] = data[j * c];
        }
    }


    int *ftout1 = 0;
    int *ftout2 = 0;
    u32 wf = 0;
    u32 hf = 0;
    g_haarlike.process(gm, nc, nr, &ftout1, &ftout2, &wf, &hf, 2, 1);

#if DEBUG
    printf("%d %d %p, %p  [%d]", wf, hf, ftout1, ftout2, ftout1[0]);
    for(int i = 0; i < hf; i ++) {
        for(int j = 0; j < wf; j ++) {
            printf("%02x ", ftout1[i * wf  + j]);
        }
        printf("\n");
    }
#endif

    int fn_len = strlen(file);
    char fn_out1[fn_len + 32];
    char fn_out2[fn_len + 32];

    memset(fn_out1, 0, fn_len + 32);
    memset(fn_out2, 0, fn_len + 32);

    snprintf(fn_out1, fn_len + 32, "%s.1.feature", file);
    snprintf(fn_out2, fn_len + 32, "%s.2.feature", file);

    write_file(fn_out1, ftout1, wf * hf * sizeof(int));
    write_file(fn_out2, ftout2, wf * hf * sizeof(int));



#if DEBUG
    IplImage* p_frame = cvCreateImage(cvSize(nc, nr), IPL_DEPTH_8U, 1);
    memcpy(p_frame->imageData, gm, p_frame->width * p_frame->height * p_frame->nChannels);
    cvNamedWindow("show", 1);
    while(1) {
        cvShowImage("show", p_frame);
        char k = cvWaitKey(33);
    }
#endif

    return 0;
}
