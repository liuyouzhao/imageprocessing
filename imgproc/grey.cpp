#include <stdio.h>
#include <string.h>
#include "proc.h"

/*
* Main processing function
*/
static unsigned char* __grey_process(u8* p, u8* out, u32 w, u32 h, int type) {

    if(!p || !out) {
        return  NULL;
    }

    if(type == 1) {
        memset(out, w * h, 0);
        for(u32 i = 0; i < h * w; i ++) {
            u8 r = p[i * 3];
            u8 g = p[i * 3 + 1];
            u8 b = p[i * 3 + 2];

            u8 grey = (r + g + b) / 3;

            out[i] = grey;
        }

        return out;
    }

    memset(out, w * h * 3, 0);
    for(u32 i = 0; i < h * w * 3; i += 3) {

		u8 r = p[i];
		u8 g = p[i + 1];
		u8 b = p[i + 2];

		u8 grey = (r + g + b) / 3;

		out[i] = grey;
		out[i + 1] = grey;
		out[i + 2] = grey;

	}
    return out;
}

GreyProc g_grey = {
    .process = __grey_process
};
