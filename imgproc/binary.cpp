#include <stdio.h>
#include <string.h>
#include "proc.h"

/*
* Main processing function
*/
unsigned char* __binary_process(u8* p, u8* out, u32 w, u32 h, u8 t) {

    if(!p || !out) {
        return  NULL;
    }

#if (COLOR_RGB)
    memset(out, w * h * 3, 0);
    for(int i = 0; i < h * w * 3; i += 3) {

		u8 r = p[i];
		u8 g = p[i + 1];
		u8 b = p[i + 2];

		u8 grey = (r + g + b) / 3;
		grey = grey >= t ? 255 : 0;

		out[i] = grey;
		out[i + 1] = grey;
		out[i + 2] = grey;

	}
#elif (COLOR_ARGB)
#endif



    /*===============================================================================*/
	/* Our teacher said that all things are some "for" loop, here it is
	 * Change the code below, with your knowledge from books.
	*/

	/*=================================================================================*/
	return out;
}

BinaryProc g_binary = {
    .process = __binary_process
};
