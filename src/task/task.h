#ifndef TASK_H
#define TASK_H

#include "./process.h"
#include "../memory/paging/paging.h"

struct registers{
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

struct task{
    struct proces *process;
    struct registers registers;
    struct task *prev;
    struct task *next;
    struct paging_4gb_chunk *chunk;
};

void user_registers();
void* task_get_stack_item(struct task *task,int top);
int copy_string_from_task(struct task *task,void *virt, void*phy,uint32_t size);
struct task *task task_current_task();


#endif