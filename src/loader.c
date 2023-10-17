#include <stdio.h>
#include <string.h> 
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>

#include <dlfcn.h>
#include <sys/mman.h>

#include "utils.h"
#include "lib.h"
#include "elfinclude.h"


int main(int argc, char **argv, char **envp) {
    if (argc < 2) {
        log_error("provide file to load");
        return 1;
    }

    size_t size = 0;
    char *elf = get_file_buff(argv, &size);

    if (elf != NULL) {
        //int (*program)(int, char **, char **);
        //fread(buf, sizeof(buf), 1, elf);
        log_info("File has been read into the buffer\n");
        int (*program)(int, char **, char **) = map_elf(elf, size);
        if (program != NULL) {
            log_info(":: Run the loaded program:\n----------------\n\n");
            int status = program(argc, argv, envp);
            log_info("\n----------------\n");
            log_info(":: End of the loaded program: program returned status: %d\n", status);
            (void)status; // silly warnings!
        }
        else {
            log_error(":: Loading unsuccessful.\n");
        }

        return 0;
    }
    log_error(":: Argument were bad.\n");
    return 1;
}
