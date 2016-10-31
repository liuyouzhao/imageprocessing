#include <stdio.h>

#include "proc.h"
#include "cv.h"
#include "highgui.h"


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
