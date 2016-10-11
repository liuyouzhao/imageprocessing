#include <stdio.h>
#include <stdlib.h>
#include "proc.h"

/*
 * ii(x, y) = ii(x-1,y) + ii(x,y-1) - ii(x-1,y-1) + p[x, y]
 * rt: 0 ok, !0 err
*/
static int haarlike_integral(u8 *p, u32 *out, u32 w, u32 h) {

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
*
Edge Horizon
    --------------------------

          1      2
          |------|
          |      |
          |      |
         3|------|4
          |      |
          |      |
         5|------|6

    -------------------------

*/
static int haarlike_edge_horizon(u32 *integ, u32 w, u32 h,
                                    u32 sw, u32 sh, u8 x, u8 y,
                                    u8 bw, u8 bh, int **ftout) {

    int p135x, p246x;
    int p12y, p34y, p56y;

    int ftw = w / sw;
    int fth = h / sh;

    int ii1, ii2, ii3, ii4, ii5, ii6;

    *ftout = (int*) malloc(ftw * fth * sizeof(int));

    p12y = y - 1;
    p34y = y + (bh >> 1) - 1;
    p34y = p34y < 0 ? 0 : p34y;
    p56y = y + bh - 1;
    for(int i = 0; i < fth; i ++, p12y += sh, p34y += sh, p56y += sh) {

        p135x = x - 1;
        p246x = x + bw - 1;
        for(int j = 0; j < ftw; j ++, p135x += sw, p246x += sw) {

            ii1 = (p12y < 0 || p135x < 0) ? 0 : integ[p12y * w + p135x];
            ii2 = p12y < 0 ? 0 : integ[p12y * w + p246x];
            ii3 = p135x < 0 ? 0 : integ[p34y * w + p135x];
            ii4 = integ[p34y * w + p246x];
            ii6 = integ[p56y * w + p246x];
            ii5 = p135x < 0 ? 0 : integ[p56y * w + p135x];

            //int ii1 = integ[p12y * w + p135x];
            //int ii2 = integ[p12y * w + p246x];
            //int ii3 = integ[p34y * w + p135x];
            //ii4 = integ[p34y * w + p246x];
            //ii5 = integ[p56y * w + p135x];
            //ii6 = integ[p56y * w + p246x];

            (*ftout)[i * ftw + j] = (-1) *   (ii3 + ii6 - ii4 - ii5) +
                                           (ii1 + ii4 - ii2 - ii3);
            //(*ftout)[i * ftw + j] = (*ftout)[i * ftw + j] / (bw * bh);

        }
    }
    return fth * ftw;
}

/*
*
Edge Horizon
    --------------------------

          1      2      3
          |------|------|
          |      |      |
          |      |      |
          |------|------|
          4      5      6
    -------------------------

*/
static int haarlike_edge_vert(u32 *integ, u32 w, u32 h, u32 sw, u32 sh, u8 x, u8 y, u8 bw, u8 bh, int **ftout) {

    int p14x, p25x, p36x;
    int p123y, p456y;

    int ftw = w / sw;
    int fth = h / sh;

    int ii1, ii2, ii3, ii4, ii5, ii6;

    *ftout = (int*) malloc(ftw * fth * sizeof(int));

    if(bw > 1)
    {
        p123y = y - 1;
        p456y = y + bh - 1;
        for(int i = 0; i < fth; i ++, p123y += sh, p456y += sh)
        {

            p14x = x - 1;
            p25x = x + (bw >> 1) - 1;
            p25x = p25x < 0 ? 0 : p25x;
            p36x = x + bw - 1;
            for(int j = 0; j < ftw; j ++, p14x += sw, p25x += sw, p36x += sw)
            {
                ii1 = (p123y < 0 || p14x < 0) ? 0 : integ[p123y * w + p14x];
                ii2 = p123y < 0 ? 0 : integ[p123y * w + p25x];
                ii3 = p123y < 0 ? 0 : integ[p123y * w + p36x];
                ii4 = p14x < 0 ? 0 : integ[p456y * w + p14x];
                ii5 = integ[p456y * w + p25x];
                ii6 = integ[p456y * w + p36x];

                (*ftout)[i * ftw + j] = (ii1 + ii5 - ii2 - ii4) +
                                        (-1) * (ii2 + ii6 - ii3 - ii5);
                //(*ftout)[i * ftw + j] = (*ftout)[i * ftw + j] / (bw * bh);
            }
        }
    }
    else {


    }


    return ftw * fth;
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

HaarlikeProc g_haarlike = {
    .haarlike_integral = haarlike_integral,
    .haarlike_edge_horizon = haarlike_edge_horizon,
    .haarlike_edge_vert = haarlike_edge_vert
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
