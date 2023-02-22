#include <dlfcn.h>
#include <sys/mman.h>
#include <elf.h>
#include <unistd.h>
#include <stdbool.h>

#include <string.h> 
#include <stdio.h>
#include <stdlib.h>

#define SEEK_END 2

#undef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20

// for some problems with allignment if there are any
inline void *base_of_page(const void* ptr) {
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    return (void *)((size_t)ptr & ~(page_size - 1));
}

// file to buff and return the buff
void *get_file_buff(char** argv, size_t *size) {
    FILE *elf_file = fopen(argv[1], "rb");
    fseek(elf_file, 0, SEEK_END);
    size_t file_size = ftell(elf_file);
    rewind(elf_file);
    char *elf = (char *)malloc(file_size);
    fread(elf, 1, file_size, elf_file);
    *size = file_size;
    fclose(elf_file);
    return elf;
}

//validate header
bool valid_hdr(Elf64_Ehdr *hdr) {
    if (hdr->e_ident[EI_MAG0] != 0x7F     // elf file type magic numbers
        || hdr->e_ident[EI_MAG1] != 0x45 
        || hdr->e_ident[EI_MAG2] != 0x4c 
        || hdr->e_ident[EI_MAG3] != 0x46) {
            return false;
    }
    #ifdef DEBUG
        printf("[INFO] :: ELF validated.\n");
    #endif
    return true;
}

// find sym in libc
void *resolve(const char *sym, void *handle) {
    void *resolved_sym = dlsym(handle, sym);
    if (resolved_sym == NULL) {
    #ifdef DEBUG
        printf("[INFO] :: resolved sym not in libc, is a jmp label or dat\n");
    #endif
    }
    return resolved_sym;
}

// provide resolved implementation for stuff in libc
void reloc(Elf64_Shdr *shdr, const Elf64_Sym *syms, const char *strings, const char *src, char *dst) {
    void *handle = dlopen("libc.so.6", RTLD_NOW);
    Elf64_Rel *rel = (Elf64_Rel *)(src + shdr->sh_offset);
    for (size_t i = 0; i < shdr->sh_size / sizeof(Elf64_Rel); ++i) {
        const char *sym = strings + syms[ELF64_R_SYM(rel[i].r_info)].st_name;
        if (ELF64_R_TYPE(rel[i].r_info) == R_386_JMP_SLOT 
            || ELF64_R_TYPE(rel[i].r_info) == R_386_GLOB_DAT) {
            *(Elf64_Xword *)(dst + rel[i].r_offset) = (Elf64_Xword)resolve(sym, handle);
        }
    }
}

// provide resolved implementation for stuff in libc
void reloca(Elf64_Shdr *shdr, const Elf64_Sym *syms, const char *strings, const char *src, char *dst) {
    void *handle = dlopen("libc.so.6", RTLD_NOW);
    Elf64_Rela *rel = (Elf64_Rela *)(src + shdr->sh_offset);
    for (size_t i = 0; i < shdr->sh_size / sizeof(Elf64_Rela); ++i) {
        const char *sym = strings + syms[ELF64_R_SYM(rel[i].r_info)].st_name;
        if (ELF64_R_TYPE(rel[i].r_info) == R_386_JMP_SLOT 
            || ELF64_R_TYPE(rel[i].r_info) == R_386_GLOB_DAT) {
            *(Elf64_Xword *)(dst + rel[i].r_offset) = (Elf64_Xword)resolve(sym, handle);
        }
    }
}

// conversion is ok "i" wont be large -1 is handy 
int locate_global_syms(Elf64_Ehdr *hdr, Elf64_Shdr *shdr) {
    for (size_t i = 0; i < hdr->e_shnum; ++i) {
        if (shdr[i].sh_type == SHT_DYNSYM) {
            return i;
        }
    }
    return -1;
}

// conversion is ok "i" wont be large -1 is handy 
int locate_syms(Elf64_Ehdr *hdr, Elf64_Shdr *shdr) {
    for (size_t i = 0; i < hdr->e_shnum; ++i) {
        if (shdr[i].sh_type == SHT_SYMTAB) {
            return i;
        }
    }
    return -1;
}

// find symbol (name) in ELF
void *find_sym(const char *name, Elf64_Shdr *shdr, Elf64_Shdr *shdr_sym, const char *src, char *dst) {
    const char *strings = src + shdr[shdr_sym->sh_link].sh_offset;
    Elf64_Sym *syms = (Elf64_Sym *)(src + shdr_sym->sh_offset);
    for (int i = 0; i < shdr_sym->sh_size / sizeof(Elf64_Sym); ++i) {
        if (strcmp(name, strings + syms[i].st_name) == 0) {
            return dst + syms[i].st_value;
        }
    }
    return NULL;
}

// alloc mem for exec to be mapped to
char *alloc_exec(size_t size) {
    char *exec = (char *)mmap(NULL, size, 
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (exec == MAP_FAILED || !exec) {
    #ifdef DEBUG
        printf("[ERROR] ::  mmap fail [errno] :: %s\n", strerror(errno));
    #endif
        return NULL;
    }
#ifdef DEBUG
    printf("[INFO] :: mmap done, resulting address of exec is: %p\n", (void *)exec);
#endif
    memset(exec, 0x0, size);
    return exec;
}

void *map_elf(char *elf_start, size_t size) {
    // buffer is now an elf
    Elf64_Ehdr *hdr = (Elf64_Ehdr *)elf_start;
    //validate the hdr
    if (!valid_hdr(hdr)) {
    #ifdef DEBUG
        printf("[ERROR] :: Invalid ELF image\n");
    #endif
        return NULL;
    }

    // alloc mem for the actual mapped file
    char *exec = alloc_exec(size);  // start of the exec
    Elf64_Phdr * phdr = (Elf64_Phdr *)(elf_start + hdr->e_phoff);
    char *start = NULL;
    char *taddr = NULL; // virtual address of target segments
    for (size_t i = 0; i < hdr->e_phnum; ++i) {
        if (phdr[i].p_type != PT_LOAD) {
            continue;
        }

        if (phdr[i].p_filesz > phdr[i].p_memsz) {
            munmap(exec, size);
        #ifdef DEBUG
            printf("[INFO] :: Image_load:: p_filesz > p_memsz (%ld > %ld)\n", phdr[i].p_filesz, phdr[i].p_memsz);
            printf("[INFO] :: munmap done\n");
        #endif
        }

        if (!(phdr[i].p_filesz)) {
            continue;
        }

        start = elf_start + phdr[i].p_offset;
        taddr = exec + phdr[i].p_vaddr; //changer p_memsz to p_vaddr
        memmove(taddr, start, phdr[i].p_filesz);
        if (!(phdr[i].p_flags & PF_W) || !(phdr[i].p_flags & PF_X)) {
            // size_t page_base_addr = pageof((void *)taddr);
            // size_t offset_from_page_begin = phdr[i].p_memsz - page_base_addr;
            // mprotect((unsigned char *)page_base_addr,
            //         phdr[i].p_memsz, 
            //         PROT_READ | PROT_EXEC);
            // if ((size_t)taddr % 0x1000 == 0) mprotect((void *)taddr, phdr[i].p_memsz, PROT_READ | PROT_EXEC);
            mprotect((void *)taddr, phdr[i].p_memsz, PROT_READ | PROT_EXEC);
        }
    }

    // find section header
    Elf64_Shdr *shdr = (Elf64_Shdr *)(elf_start + hdr->e_shoff);
    int global_symbol_table_index = locate_global_syms(hdr, shdr);
    // get global symbols and global strings from section header
    Elf64_Sym *global_syms = (Elf64_Sym *)(elf_start + shdr[global_symbol_table_index].sh_offset);
    char *global_strings = elf_start + shdr[shdr[global_symbol_table_index].sh_link].sh_offset;
    // reloc all sections that are marked as relocation entries to map to valid implementation of symbols
    for (size_t i = 0; i < hdr->e_shnum; ++i) {
        if (shdr[i].sh_type == SHT_REL)
            reloc(shdr + i, global_syms, global_strings, elf_start, exec);

        if (shdr[i].sh_type == SHT_RELA)
            reloca(shdr + i, global_syms, global_strings, elf_start, exec);
    }
    // get index of symbol table
    int symbol_table_index = locate_syms(hdr, shdr);
    if (symbol_table_index == -1) 
        return NULL;

    //find entry point of program
    void *entry = find_sym("main", shdr, shdr + symbol_table_index, elf_start, exec);
#ifdef DEBUG
    printf("[INFO] :: entry of mapped progrma localised at %p\n", entry);
#endif
    return entry;
}
