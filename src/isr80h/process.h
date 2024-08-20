#ifndef PROCESS_H
#define PROCESS_H

#include "../task/task.h"

void* isr80h_command6_process_load_start(struct interrupt_frame* frame);

#endif