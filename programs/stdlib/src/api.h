#ifndef API_H
#define API_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


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
int getkey();
void system_putchar(char c);
void* system_malloc(size_t size);
void system_free(void *addr);
void process_load_start(char *filename);
int invoke_system_command(struct command_argument *argument);
void process_get_arguments(struct process_arguments *arguments);
void exit();
int getkeyblock();
void terminal_readline(char *out, int max, bool output_while_typing);
int system_run(char *command);
struct command_argument *parse_command(char *command, int max);
int shmget(uint16_t shmkey,size_t size,uint8_t flags);
void* shmat(uint16_t shmid);
int shmdt(void *virt_addr);
int shmdestroy(void *virt_addr);
int acquire_lock(int lock_type,int shm_id);
int release_lock(int lock_type,int shm_id);


#endif