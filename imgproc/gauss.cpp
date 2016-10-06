#include <stdio.h>
#include <string.h>
#include <math.h>
#include "proc.h"

extern ConvoluteProc g_convolute;
/*
* Main processing function
*/
static unsigned char*  __gauss_process(u8 *p, u8 *out, u32 w, u32 h, int ow) {


	u8 *opera = NULL;
	float fix = 0;

	/* 3x3 guass */
	if(ow == 3) {
		u8 _opera[9] = {
			1, 2, 1,
			2, 4, 2,
			1, 2, 1
		};
		fix = 1.0f/16.0f;
		opera = _opera;
	}
	else if(ow == 5) {
		u8 _opera[25] = {
			1, 4, 7, 4, 1,
			4, 16, 26, 16, 4,
			7, 26, 41, 26, 7,
			4, 16, 26, 16, 4,
			1, 4, 7, 4, 1
		};
		fix = 1.0f/273.0f;
		opera = _opera;
	}

	g_convolute.process(p, out, w, h, opera, ow, fix);

	/*=================================================================================*/
	return out;
}

GaussProc g_gauss = {
    .process = __gauss_process
};
