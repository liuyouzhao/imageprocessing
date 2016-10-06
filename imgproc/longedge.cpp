#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include "proc.h"

/*
* Main processing function
*/
static unsigned char* __longedge_process(u8* p, u8* out, u32 w, u32 h, u8 t) {

    if(!p || !out) {
        return  NULL;
    }
    int height = h;
    int width = w;

    u64 _ml = 0xffffffff00000000;
    u64 _mr = 0x00000000ffffffff;

    //var resultPixels = [];
	u8 arr2d[h][w];
	u8 arr2dMark[h][w];
	u8 arr2dResult[h][w];

	for(int i = 0; i < height; i ++) {
		for(int j = 0; j < width; j ++) {
			int index = i * width + j;
			arr2d[i][j] = p[index * 3];
			arr2dMark[i][j] = 0;
			arr2dResult[i][j] = 0;
		}
	}

	for(int i = 0; i < height; i ++) {
		for(int j = 0; j < width; j ++) {
			if(arr2d[i][j] == 0 || arr2dMark[i][j] == 1) {
				continue;
			}

			/// start...
			std::queue<u64> que;
			std::queue<u64> saver;
			u64 pos = (((u64)(j) << 32) & _ml) + (i);
			que.push(pos);
			saver.push(pos);
			while(que.size() > 0) {

				u64 coo = que.front();
				que.pop();
				int x = (coo & _ml) >> 32;
				int y = coo & _mr;
				/// up
				if((y - 1) >= 0 && arr2d[y - 1][x] == 255 && arr2dMark[y - 1][x] == 0) {
                    u64 pos = (((u64)x << 32) & _ml) + (y - 1);
					que.push(pos);
					saver.push(pos);
					arr2dMark[y - 1][x] = 1;
				}
				/// down

				if((y + 1) < height && arr2d[y + 1][x] == 255 && arr2dMark[y + 1][x] == 0) {
                    u64 pos = (((u64)x << 32) & _ml) + (y + 1);
					que.push(pos);
					saver.push(pos);
					arr2dMark[y + 1][x] = 1;
				}
				/// left
				if((x - 1) >= 0 && arr2d[y][x - 1] == 255 && arr2dMark[y][x - 1] == 0) {
                    u64 pos = (((u64)(x - 1) << 32) & _ml) + (y);
					que.push(pos);
					saver.push(pos);
					arr2dMark[y][x - 1] = 1;
				}
				/// right
				if((x + 1) < width && arr2d[y][x + 1] == 255 && arr2dMark[y][x + 1] == 0) {
                    u64 pos = (((u64)(x + 1) << 32) & _ml) + (y);
					que.push(pos);
					saver.push(pos);
					arr2dMark[y][x + 1] = 1;
				}

				/// up-left
				if(x - 1 >= 0 && y - 1 >= 0 && arr2d[y - 1][x - 1] == 255 && arr2dMark[y - 1][x - 1] == 0) {
					u64 pos = (((u64)(x - 1) << 32) & _ml) + (y - 1);
					que.push(pos);
					saver.push(pos);
					arr2dMark[y - 1][x - 1] = 1;
				}

				/// up-right
				if(x + 1 < width && y - 1 >= 0 && arr2d[y - 1][x + 1] == 255 && arr2dMark[y - 1][x + 1] == 0) {
					u64 pos = (((u64)(x + 1) << 32) & _ml) + (y - 1);
					que.push(pos);
					saver.push(pos);
					arr2dMark[y - 1][x + 1] = 1;
				}

				/// down-left
				if(x - 1 >= 0 && y + 1 < height && arr2d[y + 1][x - 1] == 255 && arr2dMark[y + 1][x - 1] == 0) {
					u64 pos = (((u64)(x - 1) << 32) & _ml) + (y + 1);
					que.push(pos);
					saver.push(pos);
					arr2dMark[y + 1][x - 1] = 1;
				}

				/// down-right
				if(x + 1 < width && y + 1 < height && arr2d[y + 1][x + 1] == 255 && arr2dMark[y + 1][x + 1] == 0) {
					u64 pos = (((u64)(x + 1) << 32) & _ml) + (y + 1);
					que.push(pos);
					saver.push(pos);
					arr2dMark[y + 1][x + 1] = 1;
				}

			}
			if(saver.size() >= t) {
				while(saver.size() > 0) {
					u64 coo = saver.front();
					saver.pop();
					u32 x = (coo & _ml) >> 32;
					u32 y = coo & _mr;
					arr2dResult[y][x] = 255;
				}
				//saver.clear();
			}
		}
	}
	for(int i = 0; i < height; i ++) {
		for(int j = 0; j < width; j ++) {
			int index = i * width + j;
			out[index * 3] = arr2dResult[i][j];
			out[index * 3 + 1] = arr2dResult[i][j];
			out[index * 3 + 2] = arr2dResult[i][j];
		}
	}

	return out;
}

LoneedgeProc g_longedge = {
    .process = __longedge_process
};
