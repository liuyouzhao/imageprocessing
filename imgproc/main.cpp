#include <stdio.h>
#include <string.h>
#include "proc.h"

extern void process_main();
extern int feature_main(const char* file);
extern int adaboost_train_main();
extern int adaboost_test_main();

int main(int argc, char** argv)
{
    adaboost_train_main();
    //adaboost_test_main();
    return 0;
}
