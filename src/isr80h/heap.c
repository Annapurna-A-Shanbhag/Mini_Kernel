#include "heap.h"

void* isr80h_command4_malloc()
{
    size_t size = (size_t)task_get_stack_item(task_current_task(), 0);
    return process_malloc(task_current_task()->process, size);
    return 0;
    
}

void* isr80h_command5_free()
{
    void *ptr = task_get_stack_item(task_current_task(), 0);
    process_free(task_current_task()->process, ptr);
       return 0;
    
}