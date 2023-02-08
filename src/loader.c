#include <stdio.h>
#include <string.h> 
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>

#include <dlfcn.h>
#include <sys/mman.h>
#include <elf.h>

#include "utils.h"
#include "lib.h"


int main(int argc, char **argv, char **envp) {
    if (argc < 2) {
        fprintf(stderr, "provide file to load");
        return 1;
    }

    size_t size = 0;
    char *elf = get_file_buff(argv, &size);

    if (elf != NULL) {
        //int (*program)(int, char **, char **);
        //fread(buf, sizeof(buf), 1, elf);
    #ifdef DEBUG
        printf("[INFO] :: File has been read into the buffer\n");
    #endif
        int (*program)(int, char **, char **) = map_elf(elf, size);
        if (program != NULL) {
    #ifdef DEBUG
        printf("[INFO] :: Run the loaded program:\n----------------\n\n");
    #endif
        int status = program(argc, argv, envp);
    #ifdef DEBUG
        printf("\n----------------\n[INFO] :: End of the loaded program: program returned status: %d\n", status);
    #endif
        }
        else {
    #ifdef DEBUG
        printf("[ERROR] :: Loading unsuccessful. \n");
    #endif
        }
        return 0;
    }
#ifdef DEBUG
    printf("[ERROR] :: Arguments were bad");
#endif
    return 1;
}
