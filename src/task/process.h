#ifndef PROCESS_H
#define PROCESS_H

#include<stdint.h>
#include<stddef.h>

#include "../config.h"
#include "../status.h"
#include "../string/string.h"
#include "task.h"
#include "../fs/file.h"

typedef unsigned char  PROCESS_FILETYPE;
#define PROCESS_BINARY_FILE 1;
#define PROCESS_ELF_FILE 0;

struct process_allocation{
    void* addr;
    uint32_t size;
};

struct process{
    int pid;
    void *phy_addr;
    void *stack_phy_address;
    struct task *task;
    char filenmae[PATH_NAME_LIMIT];
    PROCESS_FILETYPE filetype;
    uint32_t size;
    struct process_allocation allocations[NUMBER_OF_PROCESS_ALLOCATION];
};




#endif