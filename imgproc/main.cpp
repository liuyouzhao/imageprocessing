#include <stdio.h>
#include <string.h>
#include "proc.h"

extern void process_main();
extern int feature_main(const char* file);
extern int adaboost_main();

int main(int argc, char** argv)
{
    adaboost_main();
    return 0;
}
