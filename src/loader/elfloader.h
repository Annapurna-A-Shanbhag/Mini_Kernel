#ifndef ELFLOADER_H
#define ELFLOADER_H

#include "../config.h"
#include "../memory/heap/kheap.h"
#include "../fs/file.h"
#include "elf.h"

struct elf_file
{
    char filename[PATH_LIMIT];
    void *virtual_start_address;
    void *virtual_end_address;
    void *physical_start_address;
    void *physical_end_address;
    void *elf_memory;
    int memory_size;
};

void elf_close(struct elf_file *file);
int elf_load(char *filename, struct elf_file **elf);

#endif