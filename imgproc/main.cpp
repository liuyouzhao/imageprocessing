#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proc.h"

extern void process_main();
extern int feature_main(const char* file);
extern int adaboost_train_main();
extern int adaboost_test_main();
extern int haarlike_integral(u8 *p, u32 *out, u32 w, u32 h);

extern HaarlikeProc g_haarlike;
int main(int argc, char** argv)
{
#if 0
    u32 inte[9] = {
        1, 2, 3,
        2, 4, 6,
        3, 6, 9
    };
    int* fv = 0;
    int siz = g_haarlike.haarlike_edge_horizon(inte, 3, 3, 3, 3, 2, 1, 2, 1, &fv);
    for(int i = 0; i < siz; i ++)
    {
        printf("%d\n", fv[i]);
    }
    return 0;

#endif


#if 0
    struct __feature_value **ok = (struct __feature_value**)malloc(10 * sizeof(struct __feature_value*));

    for(int i = 0; i < 10; i ++)
    {
        ok[i] = new __feature_value();
        ok[i]->value = 10 - i;
        ok[i]->sample_id = i;
    }

    sort_fv_by_val(ok, 0, 9);

    for(int i = 0; i < 10; i ++)
    {
        printf("%d %d\n", ok[i]->value, ok[i]->sample_id);
    }
    return 0;
#endif
/*
    u8 img[30] = {
        1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1
    };
    u32 inte[30] = {0};
    haarlike_integral(img, inte, 6, 5);

    for(int i = 0; i < 30; i ++)
        printf("%d ", inte[i]);


        return 0;

    int s = 2;
    int t = 2;
    int c = 0;
    for(int i = s ; i <= 24; i += s ) {

        for(int j = t; j <= 24; j += t ) {


            for(int y = 0; y < 24 - i + 1; y ++ ) {
                for(int x = 0; x < 24 - j + 1; x ++) {
                    c ++;
                }

            }
        }
    }
    printf("%d \n", c);

    retu*/
#if LETS_GO_TEST
    adaboost_test_main();
#else
#if PROCESS_SHOW
    process_main();
#else
    adaboost_train_main();
#endif
#endif
    //adaboost_train_main();


    return 0;
}
