#ifndef TASK_H
#define TASK_H

#include "./process.h"
#include "../memory/paging/paging.h"
#include "../idt/idt.h"

struct interrupt_frame;
struct registers
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct task
{
    struct process *process;
    struct registers registers;
    struct task *prev;
    struct task *next;
    struct paging_4gb_chunk *chunk;
};

struct process;

void user_registers();
int task_switch(struct task *task);
struct task *task_new(struct process *process);
int task_page();
void *task_get_stack_item(struct task *task, int top);
int copy_string_from_task(struct task *task, void *virt, void *phy, uint32_t size);
struct task *task_current_task();
void task_free(struct task *task);
void task_current_save_state(struct interrupt_frame *frame);
void task_next();
void task_free(struct task *task);
struct task *task_get_next();
void task_page_task(struct task *task);
void task_return(struct registers* reg);
void task_run_first_ever_task();
void tasks(struct task *task);



#endif