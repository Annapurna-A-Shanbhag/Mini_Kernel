#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stddef.h>

#include "../config.h"
#include "../status.h"
#include "../string/string.h"
#include "task.h"
#include "../fs/file.h"
#include "../loader/elfloader.h"
#include "../kernel.h"


#define PROCESS_BINARY_FILE 1
#define PROCESS_ELF_FILE 0

typedef unsigned char PROCESS_FILETYPE;

struct process_allocation
{
    void *addr;
    uint32_t size;
};

struct command_arguments
{
    char argument[512];
    struct command_arguments *next;
};

struct process_arguments
{
    int argc;
    char **argv;
};

struct process
{
    int pid;
    union
    {
        void *phy_addr;
        struct elf_file *elf_file;
    };

    void *stack_phy_address;
    struct task *task;
    char filename[PATH_NAME_LIMIT];
    PROCESS_FILETYPE filetype;
    uint32_t size;
    struct process_allocation allocations[NUMBER_OF_PROCESS_ALLOCATION];
    struct process_arguments arguments;
    struct keyboard_buffer
    {
        char buffer[KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
};

void* process_malloc(struct process *process, size_t size);
int process_load_switch(char *filename, struct process **process);
int process_free(struct process *process, void *ptr);
void process_get_arguments(struct process *process, int *argc, char ***argv);
int process_terminate(struct process *process);
struct process * process_current();
int process_inject_arguments(struct process *process, struct command_arguments *root_argument);

#endif