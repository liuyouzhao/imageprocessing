#if 0
#include <stdio.h>
#include <stdlib.h>
#include "proc.h"
#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"

#define CLSSF_PATH "/train/clssf/"
#define F_VAL_PATH "/train/f_v/"

extern HaarlikeProc g_haarlike;


static __clssf* gen_clssf(u8 px, u8 py, u8 bw, u8 bh, u8 tt, u8 tn) {
    return 0;
}

static int write_file(const char* file, int *data, int len) {
    FILE* fd = fopen(file, "w");
    int nblock = fwrite(data, len, 1, fd);

    fflush(fd);
    fclose(fd);

    printf("%s\n", file);
}

static int save_feature_val(const char* group, int *data, int len) {
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

    int s = 1;
    int t = 2;

    int *ftout1 = 0;
    int *ftout2 = 0;
    u32 wf = 0;
    u32 hf = 0;
    u32 *integ = 0;
    int bw, bh;

    integ = (u32*) malloc(w * h * sizeof(u32));

    ret = g_haarlike.haarlike_integral(p, integ, w, h);
    //u32 *integ, u32 w, u32 h, int **ftout, u32 *wf, u32 *hf, int bw, int bh)

    int s, t;
    s = 1;
    t = 2;
    for(int i = bh ; i <= h; i += s ) {

        for(int j = bw; j <= w; j += t ) {

            edge_horizon();

        }

    }
    ret = g_haarlike.haarlike_edge_vert(gm, nc, nr, &ftout1, &wf, &hf, 2, 1);
    //g_haarlike.process(gm, nc, nr, &ftout1, &ftout2, &wf, &hf, 2, 1);

    int fn_len = strlen(file);
    char fn_out1[fn_len + 32];
    char fn_out2[fn_len + 32];

    memset(fn_out1, 0, fn_len + 32);
    memset(fn_out2, 0, fn_len + 32);

    snprintf(fn_out1, fn_len + 32, "%s.1.feature", file);
    snprintf(fn_out2, fn_len + 32, "%s.2.feature", file);

    write_file(fn_out1, ftout1, wf * hf * sizeof(int));
    write_file(fn_out2, ftout2, wf * hf * sizeof(int));

    free(ftout1);
    free(ftout2);

    return 0;
}
#endif
