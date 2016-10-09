#include <stdio.h>
#include <string.h>
#include "proc.h"

extern void process_main();
extern int feature_main(const char* file);

int main(int argc, char** argv)
{

    //feature_main("./res/faces/face00065.bmp");
#if 1
    printf("%s %s\n", argv[0], argv[1], argv[2]);
    char* file = argv[1];

    if(argc >= 2 && file != 0) {
        feature_main(file);
    }
    else  {
        process_main();
    }
#endif
    return 0;
}
