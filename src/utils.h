#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>

#ifdef DEBUG
#define log_info(...) printf("[INFO] " __VA_ARGS__ )
#define log_error(...) printf("[ERROR] " __VA_ARGS__)
#else
#define log_info(...)
#define log_error(...)
#endif

void hex_dump(char *buf, int size) {
    printf("%d\n", size);
    int line_position = 0;
    for (int i = 0; i < size; i++) {
        printf("%02X ", (unsigned char)buf[i]);
        line_position++;
        if (line_position == 8) {
            printf(" ");
        }
        if (line_position == 16) {
            printf("\n");
            line_position = 0;
        }
    }
    printf("\n");
}

#endif