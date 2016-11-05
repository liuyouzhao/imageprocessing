#include <stdio.h>

#include "proc.h"
#include "cv.h"
#include "highgui.h"

#define ERROR_THRED 0.5f


void rotate_point(double centerX, double centerY, double angle, double* x, double* y)
{
    /*
    x1 = x0 * cosB - y0 * sinB
　　  y1 = x0 * sinB + y0 * cosB
    */
    double x0 = *x - centerX;
    double y0 = *y - centerY;
    double x1 = x0 * cos(-angle) - y0 * sin(-angle);
    double y1 = x0 * sin(-angle) + y0 * cos(-angle);

    x1 += centerX;
    y1 += centerY;

    *x = x1;
    *y = y1;
}


/**
matrix
(x1, y1) * (a, b) = (x2, y2)
           (d, e)

x1*a + y1*d = x2
x1*b + y1*e = y2

x1'*a + y1'*d = x2'
x1'*b + y1'*e = y2'

d = (x2' - x1'a)/y1
a = (x2 - y1*d)/x1

a = (x2 - y1*[(x2' - x1'*a)/y1']/x1
a = x2/x1 - y1*x2'/y1'/x1 + y1*x1'*a/y1'/x1
a = (x2/x1 - y1*x2'/y1'/x1) / (1 - y1*x1'/y1'/x1)

*/
void get_matrix_by_points(struct __surf_match_pair* pairs, int num)
{
    double a = 0.0f;
    double d = 0.0f;
    double b = 0.0f;
    double e = 0.0f;

    int n = 0;
    for(int i = 0; i < num; i ++)
    {
        double x1 = pairs[i].x1;
        double y1 = pairs[i].y1;
        double x2 = pairs[i].x2;
        double y2 = pairs[i].y2;

        for(int j = 0; j < num; j ++)
        {
            if(j == i)
            {
                continue;
            }
            double x1_ = pairs[j].x1;
            double y1_ = pairs[j].y1;
            double x2_ = pairs[j].x2;
            double y2_ = pairs[j].y2;

            double a_ = (x2/x1 - y1*x2_/y1_/x1) / (1 - y1*x1_/y1_/x1);
            double d_ = (x2_ - x1_*a_)/y1;

            double b_ = (y2/x1 - y1*y2_/y1_/x1) / (1 - y1*x1_/y1_/x1);
            double e_ = (y2_ - x1_*b_)/y1;

            double x22 = x1 * a_ + y1 * d_;
            double y22 = x1 * b_ + y1 * e_;

            double x22_ = x1_ * a_ + y1_ * d_;
            double y22_ = x1_ * b_ + y1_ * e_;

            if(fabs(x22 - x2) >= 10.0f || fabs(y22 - y2) >= 10.0f ||
               fabs(x22_ - x2_) >= 10.0f || fabs(y22_ - y2_) >= 10.0f)
            {
                continue;
            }
            a += a_;
            b += b_;
            d += d_;
            e += e_;
            n ++;
        }
    }
    a /= (double)n;
    d /= (double)n;
    b /= (double)n;
    e /= (double)n;

    printf("|%f %f| \n|%f %f|\n [%d]", a, b, d, e, n);
}
