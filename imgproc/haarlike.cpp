#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proc.h"

/*
 * ii(x, y) = ii(x-1,y) + ii(x,y-1) - ii(x-1,y-1) + p[x, y]
 * rt: 0 ok, !0 err
*/
int haarlike_integral(u8 *p, u32 *out, u32 w, u32 h) {

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
Edge Vertical
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
static int haarlike_edge_vert(u32 *integ, u32 w, u32 h,
                                    u32 sw, u32 sh, u8 x, u8 y,
                                    u8 bw, u8 bh, int **ftout) {

    int p135x, p246x;
    int p12y, p34y, p56y;

    int ftw = w / sw;
    int fth = h / sh;

    int ii1, ii2, ii3, ii4, ii5, ii6;

    *ftout = (int*) malloc(ftw * fth * sizeof(int));
    memset(*ftout, 0, ftw * fth * sizeof(int));

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
static int haarlike_edge_horizon(u32 *integ, u32 w, u32 h, u32 sw, u32 sh, u8 x, u8 y, u8 bw, u8 bh, int **ftout) {

    int p14x, p25x, p36x;
    int p123y, p456y;

    int ftw = w / sw;
    int fth = h / sh;

    int ii1, ii2, ii3, ii4, ii5, ii6;

    *ftout = (int*) malloc(ftw * fth * sizeof(int));
    memset(*ftout, 0, ftw * fth * sizeof(int));

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

    return ftw * fth;
}

/*
*
Linear Vertical
    --------------------------

          1      2
          |------|
          |      |
          |      |
         3|------|4
          |      |
          |      |
         5|------|6
          |      |
          |      |
         7|------|8

    -------------------------

*/
static int haarlike_linear_vert(u32 *integ, u32 w, u32 h,
                                    u32 sw, u32 sh, u8 x, u8 y,
                                    u8 bw, u8 bh, int **ftout) {

    int p1357x, p2467x;
    int p12y, p34y, p56y, p78y;

    int ftw = w / sw;
    int fth = h / sh;

    int ii1, ii2, ii3, ii4, ii5, ii6, ii7, ii8;

    *ftout = (int*) malloc(ftw * fth * sizeof(int));
    memset(*ftout, 0, ftw * fth * sizeof(int));

    p12y = y - 1;
    p34y = y + (bh / 3) - 1;
    p34y = p34y < 0 ? 0 : p34y;
    p56y = y + (bh / 3 * 2) - 1;
    p78y = y + bh - 1;
    for(int i = 0; i < fth; i ++, p12y += sh, p34y += sh, p56y += sh, p78y += sh) {

        p1357x = x - 1;
        p2467x = x + bw - 1;
        for(int j = 0; j < ftw; j ++, p1357x += sw, p2467x += sw) {

            ii1 = (p12y < 0 || p1357x < 0) ? 0 : integ[p12y * w + p1357x];
            ii2 = p12y < 0 ? 0 : integ[p12y * w + p2467x];
            ii3 = p1357x < 0 ? 0 : integ[p34y * w + p1357x];
            ii4 = integ[p34y * w + p2467x];
            ii5 = p1357x < 0 ? 0 : integ[p56y * w + p1357x];
            ii6 = integ[p56y * w + p2467x];
            ii7 = p1357x < 0 ? 0 : integ[p78y * w + p1357x];
            ii8 = integ[p78y * w + p2467x];

            (*ftout)[i * ftw + j] =      (ii1 + ii4 - ii2 - ii3) +
                                    (-2)*(ii3 + ii6 - ii4 - ii5) +
                                         (ii5 + ii8 - ii6 - ii7);

            //printf("%d ", (*ftout)[i * ftw + j]);
            //(*ftout)[i * ftw + j] = (*ftout)[i * ftw + j] / (bw * bh);

        }
    }
    return fth * ftw;
}


/*
*
Linear Horizon
    --------------------------

          1      2      3      4
          |------|------|------|
          |      |      |      |
          |      |      |      |
          |------|------|------|
          5      6      7      8
    -------------------------

*/
static int haarlike_linear_horizon(u32 *integ, u32 w, u32 h, u32 sw, u32 sh, u8 x, u8 y, u8 bw, u8 bh, int **ftout) {

    int p15x, p26x, p37x, p48x;
    int p1234y, p5678y;

    int ftw = w / sw;
    int fth = h / sh;

    int ii1, ii2, ii3, ii4, ii5, ii6, ii7, ii8;

    *ftout = (int*) malloc(ftw * fth * sizeof(int));
    memset(*ftout, 0, ftw * fth * sizeof(int));

    p1234y = y - 1;
    p5678y = y + bh - 1;
    for(int i = 0; i < fth; i ++, p1234y += sh, p5678y += sh)
    {

        p15x = x - 1;
        p26x = x + (bw / 3) - 1;
        p26x = p26x < 0 ? 0 : p26x;
        p37x = x + (bw / 3 * 2) - 1;
        p48x = x + bw - 1;
        for(int j = 0; j < ftw; j ++, p15x += sw, p26x += sw, p37x += sw, p48x += sw)
        {
            ii1 = (p1234y < 0 || p15x < 0) ? 0 : integ[p1234y * w + p15x];
            ii2 = p1234y < 0 ? 0 : integ[p1234y * w + p26x];
            ii3 = p1234y < 0 ? 0 : integ[p1234y * w + p37x];
            ii4 = p1234y < 0 ? 0 : integ[p1234y * w + p48x];
            ii5 = p15x < 0 ? 0 : integ[p5678y * w + p15x];
            ii6 = integ[p5678y * w + p26x];
            ii7 = integ[p5678y * w + p37x];
            ii8 = integ[p5678y * w + p48x];

            (*ftout)[i * ftw + j] = (ii1 + ii6 - ii2 - ii5) +
                                    (-2) * (ii2 + ii7 - ii3 - ii6) +
                                    (ii3 + ii8 - ii4 - ii7);


            //(*ftout)[i * ftw + j] = (*ftout)[i * ftw + j] / (bw * bh);
        }
    }

    return ftw * fth;
}







/*
* Point
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
    fe1 = (1) * (ii5 + ii1 - ii2 - ii4) + (-1) * (ii6 + ii2 - ii3 - ii5) +
               (1) * (ii8 + ii4 - ii5 - ii7) + (-1) * (ii9 + ii5 - ii6 - ii8)
    fe2 = (-1) * (ii5 + ii1 - ii2 - ii4) + (1) * (ii6 + ii2 - ii3 - ii5) +
               (-1) * (ii8 + ii4 - ii5 - ii7) + (1) * (ii9 + ii5 - ii6 - ii8)


    Left+Top is the position focusing
    */
static int haarlike_point(u32 *integ, u32 w, u32 h, u32 sw, u32 sh, u8 x, u8 y, u8 bw, u8 bh, int **ftout) {

    int p147x, p258x, p369x;
    int p123y, p456y, p789y;

    int ftw = w / sw;
    int fth = h / sh;

    int ii1, ii2, ii3, ii4, ii5, ii6, ii7, ii8, ii9;

    *ftout = (int*) malloc(ftw * fth * sizeof(int));
    memset(*ftout, 0, ftw * fth * sizeof(int));

    p123y = y - 1;
    p456y = y + (bh >> 1) - 1;
    p456y = p456y < 0 ? 0 : p456y;
    p789y = y + bh - 1;
    for(int i = 0; i < fth; i ++, p123y += sh, p456y += sh, p789y += sh)
    {
        p147x = x - 1;
        p258x = x + (bw >> 1) - 1;
        p258x = p258x < 0 ? 0 : p258x;
        p369x = x + bw - 1;
        for(int j = 0; j < ftw; j ++, p147x += sw, p258x += sw, p369x += sw)
        {
            ii1 = (p123y < 0 || p147x < 0) ? 0 : integ[p123y * w + p147x];
            ii2 = p123y < 0 ? 0 : integ[p123y * w + p258x];
            ii3 = p123y < 0 ? 0 : integ[p123y * w + p369x];
            ii4 = p147x < 0 ? 0 : integ[p456y * w + p147x];
            ii5 = integ[p456y * w + p258x];
            ii6 = integ[p456y * w + p369x];
            ii7 = p147x < 0 ? 0 : integ[p789y * w + p147x];
            ii8 = integ[p789y * w + p258x];
            ii9 = integ[p789y * w + p369x];

            (*ftout)[i * ftw + j] = (1) * (ii5 + ii1 - ii2 - ii4) + (-1) * (ii6 + ii2 - ii3 - ii5) +
                                    (-1) * (ii8 + ii4 - ii5 - ii7) + (1) * (ii9 + ii5 - ii6 - ii8);
        }
    }
    return ftw * fth;
}



HaarlikeProc g_haarlike = {
    .haarlike_integral = haarlike_integral,
    .haarlike_edge_horizon = haarlike_edge_horizon,
    .haarlike_edge_vert = haarlike_edge_vert,
    .haarlike_linear_horizon = haarlike_linear_horizon,
    .haarlike_linear_vert = haarlike_linear_vert,
    .haarlike_point = haarlike_point
};
