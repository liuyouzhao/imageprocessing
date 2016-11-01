#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "proc.h"
#include "proc_utils.h"

extern HaarlikeProc g_haarlike;


int hessian_calculate_dir_haar_addons(int x, int y, int radio,
                                int angle_from, int angle_to,
                                u32* integral, int w, int h, int s, int l);
#if USE_GROUP
int s_level[3][5] = {
    //9, 15, 21, 27, 33, 39, 45, 51, 57
    {9, 15, 21, 27, 33},
    {15, 27, 39, 51, 63},
    {27, 51, 75, 99, 123}
};
#else
int s_level[5] = {
    //9, 15, 21, 27, 33, 39, 45, 51, 57
    9, 15, 21, 27, 33
};
#endif

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
static int haarlike_edge_vert(u32 *integ, int w, int h, int x, int y, int level) {

    int p135x, p246x;
    int p12y, p34y, p56y;
    int fv = 0;

    int ii1, ii2, ii3, ii4, ii5, ii6;

#if 1
    if(level % 2 == 0)
    {
        p12y = y - (level >> 1);
        p34y = y;
        p56y = y + (level >> 1);

        p135x = x - (level >> 1) - 1;
        p246x = x + (level >> 1) - 1;
    }
    else
    {
        p12y = y - (level >> 1) - 1;
        p34y = y;
        p56y = y + (level >> 1);

        p135x = x - (level >> 1) - 1;
        p246x = x + (level >> 1);
    }

#endif // 0
#if 0
    p12y = y - 1;
    p34y = y + (level >> 1) - 1;
    p56y = y + (level) - 1;

    p135x = x - 1;
    p246x = x + (level >> 1) - 1;
#endif

    ii1 = (p12y < 0 || p135x < 0) ? 0 : integ[p12y * w + p135x];
    ii2 = p12y < 0 ? 0 : integ[p12y * w + p246x];
    ii3 = p135x < 0 ? 0 : integ[p34y * w + p135x];
    ii4 = integ[p34y * w + p246x];
    ii6 = integ[p56y * w + p246x];
    ii5 = p135x < 0 ? 0 : integ[p56y * w + p135x];


    fv = (ii3 + ii6 - ii4 - ii5) +(-1) *  (ii1 + ii4 - ii2 - ii3);
    return fv;
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
static int haarlike_edge_horizon(u32 *integ, int w, int h, int x, int y, int level) {

    int p14x, p25x, p36x;
    int p123y, p456y;

    int fv = 0;

    int ii1, ii2, ii3, ii4, ii5, ii6;


#if 1
    if(level % 2 == 0)
    {
        p123y = y - (level >> 1) - 1;
        p456y = y + (level >> 1) - 1;

        p14x = x - (level >> 1);
        p25x = x;
        p36x = x + (level >> 1);
    }
    else
    {
        p123y = y - (level >> 1) - 1;
        p456y = y + (level >> 1);

        p14x = x - (level >> 1) - 1;
        p25x = x;
        p36x = x + (level >> 1);
    }

#endif
#if 0
    p123y = y - 1;
    p456y = y + (level >> 1) - 1;

    p14x = x - 1;
    p25x = x + (level >> 1) - 1;
    p36x = x + (level) - 1;
#endif

    ii1 = (p123y < 0 || p14x < 0) ? 0 : integ[p123y * w + p14x];
    ii2 = p123y < 0 ? 0 : integ[p123y * w + p25x];
    ii3 = p123y < 0 ? 0 : integ[p123y * w + p36x];
    ii4 = p14x < 0 ? 0 : integ[p456y * w + p14x];
    ii5 = integ[p456y * w + p25x];
    ii6 = integ[p456y * w + p36x];

    fv = (-1) * (ii1 + ii5 - ii2 - ii4) + (ii2 + ii6 - ii3 - ii5);

    return fv;
}


/**
det = Lxx*Lyy - (0.9*Lxy)^2

*/
#if USE_GROUP
int hessian_value(u32* integral, double* values, int w, int h, int l, int g)
#else
int hessian_value(u32* integral, double* values, int w, int h, int l)
#endif
{
    double Lxx = 0;
    double Lyy = 0;
    double Lxy = 0;
#if USE_GROUP
    int level = s_level[g][l];
#else
    int level = s_level[l];
#endif
    double l2 = (double)level * level;

    memset(values, 0.0f, sizeof(double) * w * h);

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
#if USE_GROUP
int is_hessian_max(double* vu, double* vm, double* vd, int x, int y, int w, int h, int l, int g)
#else
int is_hessian_max(double* vu, double* vm, double* vd, int x, int y, int w, int h, int l)
#endif
{

    int ct = y * w + x;
    double ifMax = vm[ct];
#if USE_GROUP
    int edge = s_level[g][PARIMITS_LAYERS_NUMBER - 1];
    int level = s_level[g][l];
#else
    int edge = s_level[PARIMITS_LAYERS_NUMBER - 1];
    int level = s_level[l];
#endif
    int levelMap = level;

    if(x <= edge / 2 - 1 || x >= w - edge / 2 - 1)
    {
        return 0;
    }

    if(y <= edge / 2 - 1 || y >= h - edge / 2 - 1)
    {
        return 0;
    }

    for(int i = -(levelMap>>1); i <= (levelMap>>1); i ++)
    {
        for(int j = -(levelMap>>1); j <= (levelMap>>1); j ++)
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

static double get_angle(int x, int y)
{
    /// 90
    if(x == 0 && y < 0)
    {
        return PI * 0.5f;
    }

    /// 180
    if(x < 0 && y == 0)
    {
        return PI;
    }

    /// 270
    if(x == 0 && y > 0)
    {
        return PI * 1.5f;
    }

    /// 360 || 0
    if(x > 0 && y == 0)
    {
        return 0;
    }

    double angle = atan((double)-y/(double)x);

    /// 0-90
    if(x > 0 && y < 0)
    {
        return angle;
    }
    /// 90-180
    if(x < 0 && y < 0)
    {
        return angle + PI;
    }
    /// 180-270
    if(x < 0 && y > 0)
    {
        return angle + PI;
    }
    /// 270-360
    if(x > 0 && y > 0)
    {
        return angle + PI * 2;
    }


}

int hessian_get_direction(int x, int y, double s, int l, u32* integral, int w, int h)
{
    static double _opera[25] =
    {
        1, 4, 7, 4, 1,
        4, 16, 26, 16, 4,
        7, 26, 41, 26, 7,
        4, 16, 26, 16, 4,
        1, 4, 7, 4, 1
    };

    int r = (int)(6.0f * s);
    int d = 2.0f * r;
    int fvX[d*d];
    int fvY[d*d];
    int step = (int)(4.0*s);
    step = (step % 2) != 0 ? step + 1 : step;
    double angles[d*d];
    double gaussw = 0.0f;

    int n = 0;
#if 1
    //printf("**************************************\n");
    for(int i = -r; i <= r; i ++)
    {
        for(int j = -r; j <= r; j ++)
        {
            if(i*i + j*j <= r * r)
            {
                int _x = x + j;
                int _y = y + i;

                int gx = 2 + ((double)j * (5.0f / (double)r));
                int gy = 2 + ((double)i * (5.0f / (double)r));
                gaussw = (double)_opera[gy * 5 + gx];

                fvX[n] =  haarlike_edge_horizon(integral, w, h, _x, _y, step);
                fvY[n] =  haarlike_edge_vert(integral, w, h, _x, _y, step);

                //printf("%d ", fvX[n]);
                //printf("%d %d=========\n", _x, _y);
                angles[n] = get_angle(fvX[n], fvY[n]);

                n ++;
            }
        }
    }
#endif
#if 0
    for(int i = -6; i <= 6; i ++)
    {
        for(int j = -6; j <= 6; j ++ )
        {
            if(i*i + j*j < 36)
            {
                int gx = 2 + ((double)j * (5.0f / (double)6));
                int gy = 2 + ((double)i * (5.0f / (double)6));
                gaussw = (double)_opera[gy * 5 + gx];

                int _x = (double)x + (double)j*(double)s;
                int _y = (double)y + (double)i*(double)s;


                fvX[n] = gaussw * haarlike_edge_horizon(integral, w, h, _x, _y, (double)4.0f*(double)s);
                fvY[n] = gaussw * haarlike_edge_vert(integral, w, h, _x, _y, (double)4.0f*(double)s);

                printf("%f ", fvX[n]);
                printf("%d %d=========\n", _x, _y);

                angles[n] = get_angle(fvX[n], fvY[n]);
                //printf("angles: %f\n", angles[n]);
                n ++;
            }
        }
    }
#endif

#if 1
    double ang1 = 0.0f;
    double ang2 = 0.0f;
    double sum_X = 0.0f;
    double sum_Y = 0.0f;
    double orientation = 0.0f;
    double max = -9999.9f;
    for(ang1 = 0; ang1 < 2 * PI; ang1 += 0.15f)
    {
        ang2 = (ang1 + PI / 3.0f > 2 * PI ? ang1 - 5.0f * PI / 3.0f : ang1 + PI / 3.0f);
        sum_X = sum_Y = 0.0f;

        for(int k = 0; k < n; k ++)
        {
            double ang = angles[k];

            if(ang1 < ang2 && ang1 < ang && ang < ang2)
            {
                double an = ang * 180.0f / PI;

                sum_X += fvX[k];
                sum_Y += fvY[k];
            }
            else if(ang2 < ang1 && ((ang > 0 && ang < ang2) || (ang > ang1 && ang < 2 * PI)))
            {
                sum_X += fvX[k];
                sum_Y += fvY[k];
            }
        }

        if(sum_X * sum_X + sum_Y * sum_Y > max && !(sum_X == 0.0f && sum_Y == 0.0f))
        {
            max = sum_X * sum_X + sum_Y * sum_Y;
            orientation = get_angle(sum_X, sum_Y);
        }

    }
    //printf("%f %f\n", sum_X, sum_Y);
    return (int)(orientation / PI * 180.0f);
#endif
#if 0
    const int GROUPS = 24;
    int sumX[GROUPS] = {0};
    int sumY[GROUPS] = {0};
    int _REGION_ANGLE = 360 / GROUPS;
    int _HALF_ANGLE = _REGION_ANGLE / 2;
    for(int i = 0; i < n; i ++)
    {
        double an = angles[i];
        int a360 = (int)(an / PI * 180.0f);
        int group = a360 / _REGION_ANGLE;

        sumX[group] += fvX[i];
        sumY[group] += fvY[i];
    }

    int max = -999999;
    int oriented = 0;
    for(int i = 0; i < GROUPS; i ++)
    {
        int v = sumX[i] * sumX[i] + sumY[i] * sumY[i];
        if(v > max)
        {
            max = v;
            oriented = get_angle(sumX[i], sumY[i]);
        }
    }

    return oriented;
#endif
#if 0
    int max_value = -9999999;
    double dir = 0.0f;
    int an = 0.0f;
    for( ; an < 360.0f; an += HESSIAN_ANGLE_JUMP )
    {
        int val = hessian_calculate_dir_haar_addons(x, y, HESSIAN_RADIO * s,
                            an, an + HESSIAN_ANGLE, integral, w, h, HESSIAN_HAAR_SIZE * s, l);
        if(max_value < val)
        {
            max_value = val;
            dir = an + HESSIAN_ANGLE * 0.5;
        }
    }
    return (int)dir;
#endif;
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

