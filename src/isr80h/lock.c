#include "lock.h"

void* isr80h_acquire_lock(){
    struct task *task=task_current_task();
    void *user_space_buffer[2];
    for(int i=0;i<2;i++){
        user_space_buffer[i]=task_get_stack_item(task,i);
    }
    
    int lock_type=(int)paging_convert_virt_to_phy(task->chunk->directory_entry,user_space_buffer[0]);
    int shm_id=(int)paging_convert_virt_to_phy(task->chunk->directory_entry,user_space_buffer[1]);
    
    int res=lock_acquire(lock_type,shm_id,task->process);
    return (void *)res;
    

}

void* isr80h_release_lock(){
    struct task *task=task_current_task();
    void *user_space_buffer[2];
    for(int i=0;i<2;i++){
        user_space_buffer[i]=task_get_stack_item(task,i);
    }
    
    int lock_type=(int)paging_convert_virt_to_phy(task->chunk->directory_entry,user_space_buffer[0]);
    int shm_id=(int)paging_convert_virt_to_phy(task->chunk->directory_entry,user_space_buffer[1]);
    
    int res=lock_release(lock_type,shm_id,task->process);
    return (void*)res;
    
}