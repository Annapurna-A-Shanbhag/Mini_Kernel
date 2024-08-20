#ifndef IO_H
#define IO_H

#include "../task/task.h"

struct interrupt_frame;
void isr80h_command1_print(struct interrupt_frame* frame);

#endif