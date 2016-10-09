#include <stdio.h>
#include <stdlib.h>
#include "proc.h"

/*
 * ii(x, y) = ii(x-1,y) + ii(x,y-1) - ii(x-1,y-1) + p[x, y]
 * rt: 0 ok, !0 err
*/
static int integral(u8 *p, u32 *out, u32 w, u32 h) {

    if(!p  || !out) {
        return -1;
    }

    for(int i = 0; i < h; i ++) {

        int r = i * w;
        int lr = (i - 1) * w;
        for(int j = 0; j < w; j ++) {

            u8 p_x_y = p[r + j];
            u32 ii_x1_y = j - 1 < 0 ? 0 : out[r + j - 1];
            u32 ii_x_y1 = i - 1 < 0 ? 0 : out[lr + j];
            u32 ii_x1_y1 = i - 1 < 0 ? 0 : (j - 1 < 0 ? 0 : out[lr + j - 1]);

            u32 ii_x_y = ii_x1_y + ii_x_y1 - ii_x1_y1 + p_x_y;
            out[r + j] = ii_x_y;
        }
    }
    return 0;
}

/*
* Grey, 1 channel
* @params
* type:
* 1, edge
* 2, conn
* 3, center
* 4, linear
*/
static int __haarlike_process(u8* p, u32 w, u32 h,
                                int **ftout1, int **ftout2,
                                u32 *wf, u32 *hf, u32 b, int type) {

    int ret = 0;
    u32 *integ = 0;
    u32 *out1 = 0;
    u32 *out2 = 0;

    int p147x, p123y;
    int p258x, p456y;
    int p369x, p789y;

    integ = (u32*) malloc(w * h * sizeof(u32));

    ret = integral(p, integ, w, h);

    /* template in whole image

    Edge & Center
    --------------------------

          1      2      3
          |------|------|
          |      |      |
          |      |5     |
         4|------|------| 6
          |      |      |
          |      |8     |
         7|------|------|9

    -------------------------
    fe_edge1 = (1) * (ii5 + ii1 - ii2 - ii4) + (-1) * (ii6 + ii2 - ii3 - ii5) +
               (1) * (ii8 + ii4 - ii5 - ii7) + (-1) * (ii9 + ii5 - ii6 - ii8)
    fe_edge2 = (-1) * (ii5 + ii1 - ii2 - ii4) + (1) * (ii6 + ii2 - ii3 - ii5) +
               (-1) * (ii8 + ii4 - ii5 - ii7) + (1) * (ii9 + ii5 - ii6 - ii8)

    fe_conn1 = (-3) * (ii5 + ii1 - ii2 - ii4) + (1) * (ii6 + ii2 - ii3 - ii5) +
                (1) * (ii8 + ii4 - ii5 - ii7) + (1) * (ii9 + ii5 - ii6 - ii8)
    fe_edge2 = (1) * (ii5 + ii1 - ii2 - ii4) + (1) * (ii6 + ii2 - ii3 - ii5) +
               (1) * (ii8 + ii4 - ii5 - ii7) + (-3) * (ii9 + ii5 - ii6 - ii8)

    Left+Top is the position focusing
    */
    int ftw = (w - b + 1);
    int fth = (h - b + 1);
    *wf = ftw;
    *hf = fth;
    *ftout1 = (int*) malloc(ftw * fth * sizeof(int));
    *ftout2 = (int*) malloc(ftw * fth * sizeof(int));

process_edge:

    p147x = p123y = 0;
    p258x = p456y = (b >> 1);
    p369x = p789y = b - 1;

    for(int i = 0; i < fth; i ++, p123y ++, p456y ++, p789y ++) {
        for(int j = 0; j < ftw; j ++, p147x ++, p258x ++, p369x ++) {

            int ii1 = integ[p123y * ftw + p147x];
            int ii2 = integ[p123y * ftw + p258x];
            int ii3 = integ[p123y * ftw + p369x];
            int ii4 = integ[p456y * ftw + p147x];
            int ii5 = integ[p456y * ftw + p258x];
            int ii6 = integ[p456y * ftw + p369x];
            int ii7 = integ[p789y * ftw + p147x];
            int ii8 = integ[p789y * ftw + p258x];
            int ii9 = integ[p789y * ftw + p369x];

            (*ftout1)[i * ftw + j] = (1) * (ii5 + ii1 - ii2 - ii4) + (-1) * (ii6 + ii2 - ii3 - ii5) +
                                  (1) * (ii8 + ii4 - ii5 - ii7) + (-1) * (ii9 + ii5 - ii6 - ii8);
            (*ftout2)[i * ftw + j] = (-1) * (ii5 + ii1 - ii2 - ii4) + (1) * (ii6 + ii2 - ii3 - ii5) +
                                  (-1) * (ii8 + ii4 - ii5 - ii7) + (1) * (ii9 + ii5 - ii6 - ii8);

        }
    }

    free(integ);
    return ret;
}

HaarlikeProc g_haarlike = {
    .process = __haarlike_process
};

/*
 * Templates:
*/
#if 0
static int __ft_edge_1[4] = {
     1, -1,
     1, -1
};
static int __ft_edge_2[4] = {
     1, 1,
     -1, -1
};
static int __ft_conner_1[4] = {
    -1, 1,
     1, 1
};
static int __ft_conner_2[4] = {
     1, 1,
     1, -1
};
static int __ft_center_1[9] = {
     1, 1, 1,
     1, -1,1,
     1, 1, 1
};
static int __ft_center_2[9] = {
     0, 1, 0
     1, -1, 1,
     0, 1, 0
};
static int __ft_linear_1[9] = {
     1, -1, 1,
     1, -1, 1,
     1, -1, 1
};
static int __ft_linear_2[9] = {
     1,  1, 1,
     -1, -1, -1,
     1,  1, 1
};
static int getft(int *out1, int *out2, u32 b, int *ft1, int *ft2, int fw) {

    static u32 fb = b / fw;

    for(u32 i = 0; i < b; i ++) {
        for(u32 j = 0; j < b; j ++) {
            u32 fx = j / fb;
            u32 fy = i / fb;
            out1[i * b + j] = ft1[fy * fw + fx];
            out2[i * b + j] = ft2[fy * fw + fx];
        }
    }
}
#endif
