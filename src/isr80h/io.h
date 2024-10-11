#ifndef IO_H
#define IO_H

#include "../task/task.h"
#include "../kernel.h"
#include "../keyboard/keyboard.h"


struct interrupt_frame;
void * isr80h_command1_print(struct interrupt_frame *frame);
void *isr80h_command2_getkey(struct interrupt_frame *frame);
void *isr80h_command3_putchar(struct interrupt_frame *frame);

#endif