#include <stdio.h>
#include <string.h>
#include <math.h>
#include "proc.h"


/*
* Main processing function
*/
static unsigned char* __gradient_process(u8* p, u8* out, u32 w, u32 h) {

	/* prewitt */
	char prewittX[9] = {
		-1, 0, 1,
		-1, 0, 1,
		-1, 0, 1
	};

	char prewittY[9] = {
		-1, -1, -1,
		 0, 0, 0,
		 1, 1, 1
	};
	/* 3 X 3 */
	u8 preW = 3;
	int width = w;
	int height = h;

	for(int i = 0, j = 0; i < height * width * 3; i += 3, j ++) {


		int redX = 0, greenX = 0, blueX = 0;
		int redY = 0, greenY = 0, blueY = 0;

		for(char y = -1; y <= 1; y ++) {
			for(char x = -1; x <= 1; x ++) {

				int curRow = j / width + y;
				int curCol = j % width + x;
				curRow = curRow < 0 ? 0 : curRow;
				curCol = curCol < 0 ? 0 : curCol;
				curRow = curRow >= height ? height - 1 : curRow;
				curCol = curCol >= width ? width - 1 : curCol;

				int curIndex = curRow * width + curCol;
				int prewittI = (y + 1) * preW + x + 1;

				redX += p[curIndex * 3] * prewittX[prewittI];
				greenX += p[curIndex * 3 + 1] * prewittX[prewittI];
				blueX += p[curIndex * 3 + 2] * prewittX[prewittI];

				redY += p[curIndex * 3] * prewittY[prewittI];
				greenY += p[curIndex * 3 + 1] * prewittY[prewittI];
				blueY += p[curIndex * 3 + 2] * prewittY[prewittI];
			}
		}

		out[i] = sqrt(redX*redX + redY*redY);
		out[i + 1] = sqrt(greenX*greenX + greenY*greenY);
		out[i + 2] = sqrt(blueX*blueX + blueY*blueY);

	}

	/*=================================================================================*/
	return out;
}

GradientProc g_gradient = {
    .process = __gradient_process
};
