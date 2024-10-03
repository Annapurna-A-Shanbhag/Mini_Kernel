#ifndef API_H
#define API_H

#include <stdint.h>
#include <stdbool.h>

#include "../../../src/task/process.h"

struct command_argument
{
    char argument[512];
    struct command_argument *next;
};

struct process_arguments
{
    int argc;
    char **argv;
};

void print(char *str);
int get_key();
void putchar(char c);
void malloc(size_t size);
void free(void *addr);
void process_load_start(char *filename);
int invoke_system_command(struct command_argument *argument);
void process_get_arguments(struct process_arguments *arguments);
void exit();
int peachos_getkeyblock();
void peachos_terminal_readline(char *out, int max, bool output_while_typing);
void peachos_process_load_start(const char *filename);

#endif