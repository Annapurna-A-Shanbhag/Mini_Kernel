#ifndef SHRDMEM_H
#define SHRDMEM_H

#include<stddef.h>
#include<stdint.h>
#include"../task/task.h"
#include"../ipc/sharedmem.h"

struct interrupt_frame;

void* isr80h_shmget(struct interrupt_frame *frame);
void* isr80h_shmat(struct interrupt_frame *frame);
void* isr80h_shmdt(struct interrupt_frame *frame);
void* isr80h_shmdestroy(struct interrupt_frame *frame);



#endif