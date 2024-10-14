#include "isr80h.h"

void isr80h_register_commands()
{
    register_isr80h_command(SYSTEM_COMMAND1_PRINT, isr80h_command1_print);
    register_isr80h_command(SYSTEM_COMMAND2_GETKEY, isr80h_command2_getkey);
    register_isr80h_command(SYSTEM_COMMAND3_PUTCHAR, isr80h_command3_putchar);
    register_isr80h_command(SYSTEM_COMMAND4_MALLOC, isr80h_command4_malloc);
    register_isr80h_command(SYSTEM_COMMAND5_FREE, isr80h_command5_free);
    register_isr80h_command(SYSTEM_COMMAND6_PROCESS_LOAD_START, isr80h_command6_process_load_start);
    register_isr80h_command(SYSTEM_COMMAND7_INVOKE_SYSTEM_COMMAND, isr80h_command7_invoke_system_command);
    register_isr80h_command(SYSTEM_COMMAND8_GET_PROGRAM_ARGUMENTS, isr80h_command8_get_program_arguments);
    register_isr80h_command(SYSTEM_COMMAND9_EXIT, isr80h_command9_exit);
    register_isr80h_command(SYSTEM_COMMAND10_SHMGET, isr80h_shmget);
    register_isr80h_command(SYSTEM_COMMAND11_SHMAT, isr80h_shmat);
    register_isr80h_command(SYSTEM_COMMAND12_SHMDT, isr80h_shmdt);
    register_isr80h_command(SYSTEM_COMMAND13_SHMDESTROY, isr80h_shmdestroy);
    register_isr80h_command(SYSTEM_COMMAND14_ACQUIRE_LOCK,isr80h_acquire_lock);
    register_isr80h_command(SYSTEM_COMMAND15_RELEASE_LOCK,isr80h_release_lock);
}



