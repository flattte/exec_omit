#include <math.h>

int printf (const char * format, ... );

int absabs(int a) { 
    if (a < 0) { 
        return -a;
    }
    return a;
}

int main(int argc, char** argv) {
    absabs(-10);
    double k = sin(343.3);
    const char* aa = "hello from other elf\n";

    printf(aa);
    return absabs((int)(0.4  * k * 10000));
}