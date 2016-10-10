
#include <stdio.h>
#include <stdlib.h>
#include "proc.h"
#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

extern int adaboost_merge_samples(const char* path, int file_num,
                                    int nw, int nh, int sw, int sh, u8 **merged);
extern int adaboost_train(u8* sample, int w, int h, int sw, int sh);


/**
#define SAMPLE_BLOCK_WIDTH 20
#define SAMPLE_NUMBER 2000
#define SAMPLE_MERGED_WIDTH 50
#define SAMPLE_MERGED_HEIGHT 40
*/
int adaboost_main() {

    u8 *merged = 0;
    adaboost_merge_samples(SAMPLE_PATH,
                            SAMPLE_NUMBER,
                            SAMPLE_MERGED_WIDTH,
                            SAMPLE_MERGED_HEIGHT,
                            SAMPLE_BLOCK_WIDTH,
                            SAMPLE_BLOCK_WIDTH, &merged);
    //adaboost_merge_samples()
    #if 0
    cv::Mat src;
    src = cv::imread("/home/hujia/workspace/imgproc/imageprocessing/imgproc/res/faces/1.bmp");

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
    adaboost_train(gm, 20, 20, 20, 20);
#endif
    adaboost_train(merged, SAMPLE_MERGED_WIDTH*SAMPLE_BLOCK_WIDTH,
                            SAMPLE_MERGED_HEIGHT*SAMPLE_BLOCK_WIDTH,
                            SAMPLE_BLOCK_WIDTH, SAMPLE_BLOCK_WIDTH);

    return 0;
}
