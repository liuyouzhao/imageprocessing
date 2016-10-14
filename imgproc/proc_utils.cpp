#include <stdio.h>
#include "proc.h"
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

