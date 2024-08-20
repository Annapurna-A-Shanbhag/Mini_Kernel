#ifndef ISR80H_H
#define ISR80H_H

#include "io.h"
#include "./heap.h"
#include "process.h"

enum systemcommands{
    SYSTEM_COMMAND1_PRINT,
    SYSTEM_COMMAND2_GETKEY,
    SYSTEM_COMMAND3_PUTCHAR,
    SYSTEM_COMMAND4_MALLOC,
    SYSTEM_COMMAND5_FREE,
    SYSTEM_COMMAND6_PROCESS_LOAD_START,
};

void isr80h_register_commands();

#endif