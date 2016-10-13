#include <stdio.h>
#include <string.h>
#include "proc.h"

extern void process_main();
extern int feature_main(const char* file);
extern int adaboost_train_main();
extern int adaboost_test_main();
extern int haarlike_integral(u8 *p, u32 *out, u32 w, u32 h);
int main(int argc, char** argv)
{
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
    adaboost_train_main();
#endif
    //adaboost_train_main();


    return 0;
}
