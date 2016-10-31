#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "proc.h"
#include "proc_utils.h"

extern HaarlikeProc g_haarlike;


int hessian_calculate_dir_haar_addons(int x, int y, int radio,
                                int angle_from, int angle_to,
                                u32* integral, int w, int h, int s);

int s_level[5] = {
    //9, 15, 21, 27, 33, 39, 45, 51, 57
    9, 15, 21, 27, 33
};

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
static int haarlike_linear_vert(u32 *integ, int w, int h, int x, int y,
                                    int level) {

    int p1357x, p2467x;
    int p12y, p34y, p56y, p78y;
    int fv = 0;

    int ii1, ii2, ii3, ii4, ii5, ii6, ii7, ii8;

    p12y = y - (level >> 1) - 1;
    p34y = y - (level / 6) - 1;
    p56y = y + (level / 6);
    p78y = y + (level >> 1);

    p1357x = x - (level / 6) - 1;
    p2467x = x + (level / 6);

    ii1 = (p12y < 0 || p1357x < 0) ? 0 : integ[p12y * w + p1357x];
    ii2 = p12y < 0 ? 0 : integ[p12y * w + p2467x];
    ii3 = p1357x < 0 ? 0 : integ[p34y * w + p1357x];
    ii4 = integ[p34y * w + p2467x];
    ii5 = p1357x < 0 ? 0 : integ[p56y * w + p1357x];
    ii6 = integ[p56y * w + p2467x];
    ii7 = p1357x < 0 ? 0 : integ[p78y * w + p1357x];
    ii8 = integ[p78y * w + p2467x];

    fv =      (ii1 + ii4 - ii2 - ii3) +
                            (-2)*(ii3 + ii6 - ii4 - ii5) +
                                 (ii5 + ii8 - ii6 - ii7);


    return fv;
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
static int haarlike_linear_horizon(u32 *integ, int w, int h, int x, int y, int level) {

    int p15x, p26x, p37x, p48x;
    int p1234y, p5678y;
    int fv = 0;

    int ii1, ii2, ii3, ii4, ii5, ii6, ii7, ii8;

    p1234y = y - (level / 6) - 1;
    p5678y = y + (level / 6);

    p15x = x - (level >> 1) - 1;
    p26x = x - (level / 6) - 1;
    p37x = x + (level / 6);
    p48x = x + (level >> 1);

    ii1 = (p1234y < 0 || p15x < 0) ? 0 : integ[p1234y * w + p15x];
    ii2 = p1234y < 0 ? 0 : integ[p1234y * w + p26x];
    ii3 = p1234y < 0 ? 0 : integ[p1234y * w + p37x];
    ii4 = p1234y < 0 ? 0 : integ[p1234y * w + p48x];
    ii5 = p15x < 0 ? 0 : integ[p5678y * w + p15x];
    ii6 = integ[p5678y * w + p26x];
    ii7 = integ[p5678y * w + p37x];
    ii8 = integ[p5678y * w + p48x];

    fv = (ii1 + ii6 - ii2 - ii5) +
                            (-2) * (ii2 + ii7 - ii3 - ii6) +
                            (ii3 + ii8 - ii4 - ii7);


    return fv;
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
static int haarlike_point(u32 *integ, int w, int h, int x, int y, int level) {

    int p_1_3_9_11_x, p_2_4_10_12_x, p_5_7_13_15_x, p_6_8_14_16_x;
    int p_1_2_5_6_y, p_3_4_7_8_y, p_9_10_13_14_y, p_11_12_15_16_y;

    int fv = 0;
    int ii1, ii2, ii3, ii4, ii5, ii6, ii7, ii8, ii9, ii10, ii11, ii12, ii13, ii14, ii15, ii16;


    p_1_3_9_11_x = x - (level >> 1);
    p_2_4_10_12_x = x - (level / 6);
    p_5_7_13_15_x = x;
    p_6_8_14_16_x = x + (level >> 1) - 1;

    p_1_2_5_6_y = y - (level >> 1);
    p_3_4_7_8_y = y - (level / 6);
    p_9_10_13_14_y = y;
    p_11_12_15_16_y = y + (level >> 1) - 1;

    ii1 = integ[p_1_2_5_6_y * w + p_1_3_9_11_x];
    ii2 = integ[p_1_2_5_6_y * w + p_2_4_10_12_x];
    ii3 = integ[p_3_4_7_8_y * w + p_1_3_9_11_x];
    ii4 = integ[p_3_4_7_8_y * w + p_2_4_10_12_x];
    ii5 = integ[p_1_2_5_6_y * w + p_5_7_13_15_x];
    ii6 = integ[p_1_2_5_6_y * w + p_6_8_14_16_x];
    ii7 = integ[p_3_4_7_8_y * w + p_5_7_13_15_x];
    ii8 = integ[p_3_4_7_8_y * w + p_6_8_14_16_x];
    ii9 = integ[p_9_10_13_14_y * w + p_1_3_9_11_x];
    ii10 = integ[p_9_10_13_14_y * w + p_2_4_10_12_x];
    ii11 = integ[p_11_12_15_16_y * w + p_1_3_9_11_x];
    ii12 = integ[p_11_12_15_16_y * w + p_2_4_10_12_x];
    ii13 = integ[p_9_10_13_14_y * w + p_5_7_13_15_x];
    ii14 = integ[p_9_10_13_14_y * w + p_6_8_14_16_x];
    ii15 = integ[p_11_12_15_16_y * w + p_5_7_13_15_x];
    ii16 = integ[p_11_12_15_16_y * w + p_6_8_14_16_x];


    fv = (1) * (ii4 + ii1 - ii2 - ii3) + (-1) * (ii8 + ii5 - ii6 - ii7) +
                                    (-1) * (ii12 + ii9 - ii11 - ii10) + (1) * (ii16 + ii13 - ii14 - ii15);

    return fv;
}

/**
det = Lxx*Lyy - (0.9*Lxy)^2

*/
int hessian_value(u32* integral, double* values, int w, int h, int l)
{
    double Lxx = 0;
    double Lyy = 0;
    double Lxy = 0;

    int level = s_level[l];
    double l2 = (double)level * level;

    memset(values, -9999, sizeof(int) * w * h);

    //l2 = 1;
    for(int i = level / 2 + 1; i < h - level / 2 - 1; i ++)
    {
        for(int j = level / 2 + 1; j < w - level / 2 - 1; j ++)
        {
            Lyy = haarlike_linear_vert(integral, w, h, j, i, level);
            Lxx = haarlike_linear_horizon(integral, w, h, j, i, level);
            Lxy = haarlike_point(integral, w, h, j, i, level);

            Lyy = Lyy / l2;
            Lxx = Lxx / l2;
            Lxy = Lxy / l2;

            values[i * w + j] = (Lxx*Lyy - (0.9*Lxy)*(0.9*Lxy));
            //printf("%d ", values[i * w + j]);
        }
    }

    return 0;
}

int is_hessian_max(double* vu, double* vm, double* vd, int x, int y, int w, int h)
{

    int ct = y * w + x;
    double ifMax = vm[ct];
    int level = s_level[PARIMITS_LAYERS_NUMBER - 1];

    if(x <= level / 2 - 1 || x >= w - level / 2 - 1)
    {
        return 0;
    }

    if(y <= level / 2 - 1 || y >= h - level / 2 - 1)
    {
        return 0;
    }

    for(int i = -(level>>1); i <= (level>>1); i ++)
    {
        for(int j = -(level>>1); j <= (level>>1); j ++)
        {
            int xc = x + j;
            int yc = y + i;
            int idex = yc * w + xc;

            if(vu[idex] > ifMax || vd[idex] > ifMax || vm[idex] > ifMax)
            {
                return 0;
            }
        }
    }

    return 1;
}


int hessian_get_direction(int x, int y, double s, u32* integral, int w, int h)
{
    int max_value = -9999999;
    double dir = 0.0f;
    int an = 0.0f;
    for( ; an < 360.0f; an += HESSIAN_ANGLE_JUMP )
    {
        int val = hessian_calculate_dir_haar_addons(x, y, HESSIAN_RADIO * s,
                            an, an + HESSIAN_ANGLE, integral, w, h, HESSIAN_HAAR_SIZE * s);
        if(max_value < val)
        {
            max_value = val;
            dir = an + HESSIAN_ANGLE * 0.5;
        }
    }
    return (int)dir;
}

/**
angle: 0---360
*/
int hessian_calculate_dir_haar_addons(int x, int y, int radio,
                                int angle_from, int angle_to,
                                u32* integral, int w, int h, int s)
{

    std::vector<long> points_in_angle;

    ///(1) get mask
    int* mask = (int*) malloc(radio * radio * sizeof(int));

    memset(mask, 0, sizeof(int) * radio * radio);

    double tan_angle_from = 0.0f;
    double tan_angle_to = 0.0f;

    tan_angle_from = tan(angle_from);
    tan_angle_to = tan(angle_to);


    int ix = 0;
    int iy = 0;
    int dbg[radio * radio * 4];
    int i = 0;
    double angle_cur = 0.0f;

    long values = ((x << 32) & 0xffffffff00000000) + y;
    points_in_angle.push_back(values);

    for( iy = y - radio ; iy < y + radio; iy ++ )
    {
        for( ix = x - radio ; ix < x + radio; ix ++)
        {
            double tan_cur = ((double)y - (double)iy) / ((double)ix - (double)x);
            angle_cur = 0;
            dbg[i] = 0;

            if((ix - x) * (ix - x) + (iy - y) * (iy - y) <= radio * radio)
            {
                if(ix >= x && iy <= y)
                {
                    if(angle_to <= 360)
                        angle_cur = atan(tan_cur) * 180.0f / PI;
                    else
                        angle_cur = atan(tan_cur) * 180.0f / PI + 360.0f;
                }
                else if(ix <= x && iy <= y)
                {
                    angle_cur = atan(tan_cur) * 180.0f / PI + 180.0f;
                }
                else if(ix >= x && iy >= y)
                {
                    angle_cur = atan(tan_cur) * 180.0f / PI + 360.0f;
                }
                else if(ix <= x && iy >= y)
                {
                    angle_cur = atan(tan_cur) * 180.0f / PI + 180.0f;
                }
                /*else
                {
                    angle_cur = atan(tan_cur);
                    if(angle_cur >= 0)
                    {
                        angle_cur = atan(tan_cur) * 180.0f / PI + 180.0f;
                    }
                    else
                    {
                        angle_cur = 360.0f - ( atan(tan_cur) * 180.0f / PI + 180.0f );
                    }
                }*/
                if(angle_cur >= angle_from && angle_cur <= angle_to)
                {
                    dbg[i] = 1;
                    long values = ((ix << 32) & 0xffffffff00000000) + iy;
                    points_in_angle.push_back(values);
                    //mask[iy * radio + ix] = 1;
                }
            }
            i ++;
        }
    }
#if 0
    //dbg[y * radio * 2 + x] = 1;
    for(int i = 0; i < radio * 2; i ++)
    {
        for(int j = 0; j < radio * 2; j ++)
        {
            printf("%d ", dbg[i * radio * 2 + j]);
        }
        printf("\n");
    }
#endif;

    int addon = 0;
    std::vector<long>::iterator it = points_in_angle.begin();
    for(; it != points_in_angle.end(); it ++)
    {
        long v = *it;
        ix = v & 0xffffffff00000000 >> 32;
        iy = v & 0x00000000ffffffff;
        int* fv = 0;
        int* fv2 = 0;

        //haarlike_edge_horizon(u32 *integ, u32 w, u32 h, u32 sw, u32 sh, u8 x, u8 y, u8 bw, u8 bh, int **ftout)
        g_haarlike.haarlike_edge_horizon(integral, w, h, w, h, ix, iy, s, s, &fv);
        g_haarlike.haarlike_edge_vert(integral, w, h, w, h, ix, iy, s, s, &fv2);

        addon += (fv)[0];
        addon += fv2[0];
        free(fv);
        free(fv2);
    }
    return addon;
}

int hessian_descript(u8* image, int w, int h, int x, int y, int dir, int* out, double s)
{
    int block = HESSIAN_DESCRIPT_BLOCK * s;
    u8 mat[block * block];
    memset(mat, 0, block * block);

    double angle = (double)dir / 360.0f * 2 * PI;

    double step_x = cos(angle);
    double step_y = -sin(angle);

    /// TODO: Here may not correct
    int x0 = x - block * 1.414 * cos(angle);
    int y0 = y - block * 1.414 * sin(angle);
    double xc = x0;
    double yc = y0;

    int tarr[w * h];
    memset(tarr, 0, w * h * sizeof(int));
    for(int i = 0; i < block; i ++)
    {
        for(int j = 0; j < block; j ++)
        {
            xc = xc >= w ? w - 1 : xc;
            yc = yc >= h ? h - 1 : yc;
            tarr[(int)yc * w + (int)xc] = 1;
            mat[i * block + j] = image[(int)yc * w + (int)xc];
            xc = (double)xc + step_x;
            yc = (double)yc + step_y;
            //printf("[%d,%d] ", x0, y0);
        }
        xc = x0 + step_x * i;
        yc = y0 - step_y * i;
    }
#if 0
    printf("================================================================\n");
    printf("%d\n", dir);
    for(int i = 0; i < h; i ++)
    {
        for(int j = 0; j < w; j ++)
        {
            printf("%d", tarr[i * w + j]);
        }
        printf("\n");
    }
#endif
    u32* region_integral = (u32*)malloc(sizeof(int) * block * block);
    g_haarlike.haarlike_integral(mat, region_integral, block, block);

    int seed_block = block / HESSIAN_DESCRIPT_NUM_BLOCK;

    int dx = 0;
    int _dx = 0;
    int dy = 0;
    int _dy = 0;
    int num = 0;
    for(int i = 0; i < block; i += seed_block)
    {
        for(int j = 0; j < block; j += seed_block)
        {
            for(int yi = i; yi < i + seed_block; yi ++)
            {
                for(int xj = j; xj < j + seed_block; xj ++)
                {
                    int* fv = 0;
                    int* fv2 = 0;
                    g_haarlike.haarlike_edge_horizon(region_integral, block, block, block, block,
                                            xj, yi, HESSIAN_DESCRIPT_HAAR_SIZE*s, HESSIAN_DESCRIPT_HAAR_SIZE*s, &fv);
                    g_haarlike.haarlike_edge_vert(region_integral, block, block, block, block,
                                            xj, yi, HESSIAN_DESCRIPT_HAAR_SIZE*s, HESSIAN_DESCRIPT_HAAR_SIZE*s, &fv2);

                    dx += fv[0];
                    _dx += fv[0] > 0 ? fv[0] : -fv[0];
                    dy += fv2[0];
                    _dy += fv2[0] > 0 ? fv2[0] : -fv2[0];

                    free(fv);
                    free(fv2);
                }
            }
            out[num] = dx;
            out[num + 1] = _dx;
            out[num + 2] = dy;
            out[num + 3] = _dy;
            num += 4;
        }
    }
    printf("============================ %d\n\n", num);

    for(int i = 0; i < 64; i ++)
    {
        printf("%d ", out[i]);
    }

    printf("\n\n");

    free(region_integral);
    return 0;
}

