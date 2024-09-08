#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stddef.h>

#include "../config.h"
#include "../status.h"
#include "../string/string.h"
#include "task.h"
#include "../fs/file.h"

typedef unsigned char PROCESS_FILETYPE;
#define PROCESS_BINARY_FILE 1;
#define PROCESS_ELF_FILE 0;

struct process_allocation
{
    void *addr;
    uint32_t size;
};

struct command_arguments
{
    char argument[512];
    struct current_arguments *next;
};

struct process_arguments
{
    int argc;
    char **argv;
};

struct process
{
    int pid;
    void *phy_addr;
    void *stack_phy_address;
    struct task *task;
    char filename[PATH_NAME_LIMIT];
    PROCESS_FILETYPE filetype;
    uint32_t size;
    struct process_allocation allocations[NUMBER_OF_PROCESS_ALLOCATION];
    struct process_arguments arguments;
    struct keyboard
    {
        char buffer[KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
};

int process_malloc(struct process *process, size_t size);
int process_load_switch(char *filename, struct process **process);
int process_free(struct process *process, void *ptr);
void process_get_arguments(struct process *process, int *argc, char ***argv);
int process_terminate(struct process *process);

#endif