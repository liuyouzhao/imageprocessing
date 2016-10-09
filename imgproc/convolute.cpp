#include <stdio.h>
#include <string.h>
#include <math.h>
#include "proc.h"


/*
* Main processing function

    u32 px00 = px01 = px02 = j - 1 < 0 ? 0 : j - 1;
    u32 px10 = px11 = px12 = j;
    u32 px20 = px21 = px22 = j + 1 >= w ? w - 1 : j + 1;

    u32 py00 = py10 = py20 = i - 1 < 0 ? 0 : i - 1;
    u32 py01 = py11 = py21 = i;
    u32 py02 = py12 = py22 = i + 1 >= h ? h - 1 : i + 1;

*/
static unsigned char* __convolute_process(u8 *p, u8 *out, u32 w, u32 h, u8 *opera, u32 ow, float fix) {

	u8 *pixels = p;
	u32 width = w;
	u32 height = h;

	/* w X w */
	int hw = ow / 2;

    /*===============================================================================*/
	/* Our teacher said that all things are some "for" loop, here it is
	 * Change the code below, with your knowledge from books.
	*/
	for(u32 i = 0, j = 0; i < height * width * 3; i += 3, j ++) {


		int red = 0, green = 0, blue = 0;

		for(int y = -hw; y <= hw; y ++) {
			for(int x = -hw; x <= hw; x ++) {

				int curRow = (j / width) + y;
				int curCol = j % width + x;
				curRow = curRow < 0 ? 0 : curRow;
				curCol = curCol < 0 ? 0 : curCol;
				curRow = curRow >= height ? height - 1 : curRow;
				curCol = curCol >= width ? width - 1 : curCol;

				int curIndex = curRow * width + curCol;
				int operaI = (y + hw) * ow + x + hw;

				red += pixels[curIndex * 3] * opera[operaI];
				green += pixels[curIndex * 3 + 1] * opera[operaI];
				blue += pixels[curIndex * 3 + 2] * opera[operaI];
			}
		}

		out[i] = (u8)((float)red * fix);
		out[i + 1] = (u8)((float)green * fix);
		out[i + 2] = (u8)((float)blue * fix);
        //printf("%d %d %d \n", out[i], green, blue);
	}


	/*=================================================================================*/
	return out;
}

ConvoluteProc g_convolute = {
    .process = __convolute_process
};
