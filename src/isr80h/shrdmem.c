#include "shrdmem.h"



void * isr80h_shmget(struct interrupt_frame *frame)
{
    struct task *task=task_current_task();
    void *user_space_msg_buffer[3];
    for(int i=0;i<3;i++){
        user_space_msg_buffer[i] = task_get_stack_item(task, i);
    }
    uint16_t shmkey= (uint16_t) (uint32_t) paging_convert_virt_to_phy(task->chunk->directory_entry,user_space_msg_buffer[0]);
    uint32_t size=(uint32_t) paging_convert_virt_to_phy(task->chunk->directory_entry,(void *)(user_space_msg_buffer[1]));
    uint8_t flags=(uint8_t) (uint32_t) paging_convert_virt_to_phy(task->chunk->directory_entry,(void *)(user_space_msg_buffer[2]));
    int res=shrdmget(shmkey,size,flags);
    if(res<0){
        return (void *)-1;
    }
    return (void *)res;

}

void* isr80h_shmat(struct interrupt_frame *frame){
    struct task *task=task_current_task();
     void *user_space_msg_buffer = task_get_stack_item(task, 0);
    uint16_t shmid=(uint16_t) (uint32_t) paging_convert_virt_to_phy(task->chunk->directory_entry,user_space_msg_buffer);
    void* res=shrdmat(shmid,task->process);
    return res;

}

void* isr80h_shmdt(struct interrupt_frame *frame){
    struct task *task=task_current_task();
     void *user_space_msg_buffer = task_get_stack_item(task, 0);
    void *phy_addr=paging_convert_virt_to_phy(task->chunk->directory_entry,user_space_msg_buffer);
    int res=shrdmdt(phy_addr,task->process);
    if(res<0){
        return (void *)-1;
    }
    return 0;
}

void * isr80h_shmdestroy(struct interrupt_frame *frame){
    struct task *task=task_current_task();
    void *user_space_msg_buffer = task_get_stack_item(task, 0);
    void *phy_addr=paging_convert_virt_to_phy(task->chunk->directory_entry,user_space_msg_buffer);
    int res=shrdmdestroy (phy_addr,task->process);
    if(res<0){
        return (void *)-1;
    }
    return 0;

}