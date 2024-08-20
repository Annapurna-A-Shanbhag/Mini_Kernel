#include "heap.h"

void isr80h_command4_malloc(){
    size_t size=(size_t)task_get_stack_item(task_current_task(),0);
    int res=process_malloc(task_current_task()->task->process,size);
    if(res<0){
        return;
    }

}

void isr80h_command5_free(){
    void* ptr=task_get_stack_item(task_current_task(),0);
    int res=process_free(task_current_task()->task->process,ptr);
    if(res<0){
        return;
    }

}