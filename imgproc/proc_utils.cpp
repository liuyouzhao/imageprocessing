#include <stdio.h>
#include "proc.h"

#include "cv.h"
#include "highgui.h"
#include "imgcodecs.hpp"
/**
* quick sort
**/
void sort(int *a, int left, int right)
{
    if(left >= right)
    {
        return ;
    }
    int i = left;
    int j = right;
    int key = a[left];

    while(i < j)
    {
        while(i < j && key <= a[j])
        {
            j--;
        }

        a[i] = a[j];


        while(i < j && key >= a[i])
        {
            i++;
        }

        a[j] = a[i];
    }

    a[i] = key;
    sort(a, left, i - 1);
    sort(a, i + 1, right);
}

/**
* quick sort
**/
void sort_fv_by_val(struct __feature_value **pfv, int left, int right)
{
    if(left >= right)
    {
        return ;
    }
    int i = left;
    int j = right;
    struct __feature_value* key = pfv[left];

    while(i < j)
    {
        while(i < j && key->value <= pfv[j]->value)
        {
            j--;
        }

        pfv[i] = pfv[j];

        while(i < j && key->value >= pfv[i]->value)
        {
            i++;
        }

        pfv[j] = pfv[i];
    }

    pfv[i] = key;

    sort_fv_by_val(pfv, left, i - 1);
    sort_fv_by_val(pfv, i + 1, right);
}



/**
* quick sort
**/
void sort_fv_by_val(struct __possi_rect **rects, int left, int right)
{
    if(left >= right)
    {
        return ;
    }
    int i = left;
    int j = right;
    struct __possi_rect* key = rects[left];

    while(i < j)
    {
        while(i < j && key->rt <= rects[j]->rt)
        {
            j--;
        }

        rects[i] = rects[j];

        while(i < j && key->rt >= rects[i]->rt)
        {
            i++;
        }

        rects[j] = rects[i];
    }

    rects[i] = key;

    sort_fv_by_val(rects, left, i - 1);
    sort_fv_by_val(rects, i + 1, right);
}


int read_file(const char* file, char* data, int len) {
    FILE* fd = fopen(file, "r");
    int siz = fread((char*)data, len, 1, fd);

    if(siz != 1) {
        printf("Read File ERROR %s %d\n", file, siz);
        fclose(fd);
        return -1;
    }

    fclose(fd);

    //printf("%s\n", file);
    return 0;
}

int write_file(const char* file, char* data, int len) {
    FILE* fd = fopen(file, "w");
    int nblock = fwrite((char*)data, len, 1, fd);

    if(nblock != 1) {
        printf("Error, write file error: %s   size: %d \n ", file, nblock);
        return -1;
    }

    fflush(fd);
    fclose(fd);

    //printf("%s\n", file);
}

int write_file_by_id(const char* header, int x, int y, int bw, int bh, int tt, int tn, char* data, int len)
{
    char file[256] = {0};
    sprintf(file, "%s/f_v_%d_%d_%d_%d_%d_%d", header, x, y, bw, bh, tt, tn);
    return write_file(file, data, len);
}


int mat_to_u8arr(cv::Mat src, u8* dst, int w, int h)
{
    if(!src.data)
    {
        printf("Error! file is empty! \n");
        return -1;
    }

    int nr = src.rows;
    int nc = src.cols;
    int c = src.channels();

    for(int i = 0; i < nr; i ++) {
        uchar* data = src.ptr<uchar>(i);
        for(int j = 0; j < nc; j ++) {
            dst[i * w + j] = data[j * c];
        }
    }
}


void show_image(u8* buffer, int w, int h)
{
    IplImage* p_frame = NULL;
    p_frame = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);

    memcpy(p_frame->imageData, buffer, p_frame->width * p_frame->height * p_frame->nChannels);

    while(1) {
        cvShowImage("show", p_frame);
        char k = cvWaitKey(33);
        if(k == 27) break;
    }
    //delete p_frame;
    //free(p_frame);
}
