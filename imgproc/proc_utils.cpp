#include <stdio.h>
#include <time.h>
#include <sys/time.h>
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

void load_grey_image_from_file(const char* file, u8** image, int *w, int *h)
{
    IplImage* pic = cvLoadImage(file);


    //show_image_c((u8*)pic->imageData, pic->width, pic->height, pic->nChannels);

    *w = pic->width;
    *h = pic->height;
    int c = pic->nChannels;
    int n = 0;
    int len = (*w) * (*h);

    *image = (u8*)malloc(len);

    int width = *w;
    int height = *h;
    for(int i = 0, j = 0; i < width*height*3; i += 3, j ++)
    {
        uchar r = pic->imageData[i];
        uchar g = pic->imageData[i + 1];
        uchar b = pic->imageData[i + 2];
        int grey = 0.30 * r + 0.59 * g + 0.11 * b;
        grey = grey > 255 ? 255 : grey;
        (*image)[j] = grey;
    }

    for(int i = 0, j = 0; i < width*height*3; i += 3, j ++)
    {
        pic->imageData[i] = (*image)[j];
        pic->imageData[i + 1] = (*image)[j];
        pic->imageData[i + 2] = (*image)[j];
    }


    //show_image_c((u8*)pic->imageData, *w, *h, 3);
#if 0
    cv::Mat src;
    src = cv::imread(file);
    if(!src.data)
    {
        printf("Error! file is empty! \n");
        return;
    }

    int nr = src.rows;
    int nc = src.cols;
    int c = src.channels();
    *image = (u8*)malloc(nr * nc);

    int n = 0;
    for(int i = 0; i < nr; i ++) {
        uchar* data = src.ptr<uchar>(i);
        for(int j = 0; j < nc; j ++) {
            int r = data[j * c];
            int g = data[j * c + 1];
            int b = data[j * c + 2];
            int grey = 0.30 * r + 0.59 * g + 0.11 * b;
            grey = grey > 255 ? 255 : grey;
            (*image)[n] = grey;
            n ++;
        }
    }
    *w = nc;
    *h = nr;
#endif
}

void load_color_image_from_file(const char* file, u8** image, int *w, int *h)
{
    cv::Mat src;
    src = cv::imread(file);
    if(!src.data)
    {
        printf("Error! file is empty! \n");
        return;
    }

    int nr = src.rows;
    int nc = src.cols;
    int c = src.channels();
    *image = (u8*)malloc(nr * nc * c);

    int n = 0;
    for(int i = 0; i < nr; i ++) {
        uchar* data = src.ptr<uchar>(i);
        for(int j = 0; j < nc; j ++) {
            uchar r = data[j * c];
            uchar g = data[j * c + 1];
            uchar b = data[j * c + 2];
            (*image)[n] = r;
            (*image)[n + 1] = g;
            (*image)[n + 2] = b;
            n = n + c;
        }
    }
    *w = nc;
    *h = nr;
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

void show_image_c(u8* buffer, int w, int h, int c)
{
    IplImage* p_frame = NULL;
    p_frame = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, c);

    memcpy(p_frame->imageData, buffer, p_frame->width * p_frame->height * p_frame->nChannels);

    while(1) {
        cvShowImage("show", p_frame);
        char k = cvWaitKey(33);
        if(k == 27) break;
    }
    //delete p_frame;
    //free(p_frame);
}



static struct timeval __start_time;
static struct timeval __end_time;

void __perf_begin_time()
{
    gettimeofday(&__start_time, NULL);
}


void __perf_end_time()
{
    gettimeofday(&__end_time, NULL);
    long seconds  = __end_time.tv_sec  - __start_time.tv_sec;
    long useconds = __end_time.tv_usec - __start_time.tv_usec;

    long mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    printf("Elapsed time: %ld milliseconds\n", mtime);
}
