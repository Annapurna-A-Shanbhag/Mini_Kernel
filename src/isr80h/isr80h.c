#include "isr80h.h"

void isr80h_register_commands()
{
    register_isr80h_command(SYSTEM_COMMAND1_PRINT, isr80h_command1_print);
    register_isr80h_command(SYSTEM_COMMAND2_GETKEY, isr80h_command2_getkey);
    register_isr80h_command(SYSTEM_COMMAND3_PUTCHAR, isr80h_command3_putchar);
    register_isr80h_command(SYSTEM_COMMAND4_MALLOC, isr80h_command4_malloc);
    register_isr80h_command(SYSTEM_COMMAND5_FREE, isr80h_command5_free);
    register_isr80h_command(SYSTEM_COMMAND6_PROCESS_LOAD_START, isr80h_command5_process_load_start);
}