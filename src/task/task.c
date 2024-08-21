#include "task.h"

struct task *task_head=0;
struct task *task_tail=0;
struct task *current_task=0;

struct task *task task_current_task(){
    return current_task;
}
void task_page_task(struct task *task){
    user_registers();
    page_switch(task->chunk);
}

struct task *task task_get_next(){
    if(!current_task->next){
        return task_head;
    }
    return current_task->next;
}

void task_list_remove(struct task *task){
    if(task->prev){
        task->prev->next=task->next;
    }
    
    if(task_head==task){
        task_head=task->next;
    }

    if(task_tail==task){
        task_tail=task->prev;
    }

    if(task==current_task){
        current_task=task_get_next();
    }

}

void task_free(struct task *task){
    paging_free_4gb_chunk(task->chunk);
    task_list_remove(task);
    kfree(task);
}

void* task_get_stack_item(struct task *task,int top){
    void* result=0;
    uint32_t *sp_ptr=task->registers.esp;
    task_page_task(task);
    result=sp_ptr[top];
    rkernel_page();
    return result;
}

int copy_string_from_task(struct task *task,void *virt, void*phy,uint32_t size)
{
    int res=0;
    if(size>PAGE_SIZE){
        res-EINVARG;
        goto out;
    }
    void *tmp=kzalloc(sizeof(size));
    if(!tmp){
        res-ENOMEM;
        goto out;
    }
    int old_value=page_get(task->chunk,tmp);
    if(old_value<0){
        goto out;
    }
    task_page_task(task);
    paging_map_to(task->chunk,tmp,tmp,size,PAGING_IS_PREENT|PAGING_IS_WRITABLE|PAGING_ACCESS_FROM_ALL);
    str_n_cpy(tmp,virt,size);
    kernel_page();
    res=paging_set(task->chunk->directory_entry,tmp,old_value);
    if(res<0){
        res=-EIO;
        goto out;
    }
    str_n_cpy(phy,tmp,size);


out:return res;
}

int task_initialization(struct task *task,struct process *process){
    task->chunk=paging_new_4gb(PAGING_IS_PREENT|PAGING_IS_WRITABLE|PAGING_ACCESS_FROM_ALL);
    if(!task->chunk){
        return -ENOMEM;
    }
    task->process=process;
    task->registers.ip=PROGRAM_VIRTUAL_ADDRESS;
    task->registers.esp=PROGRAM_STACK_VIRT_ADDRESS;
    task->registers.cs=KERNEL_CODE_SEGMENT_SELECTOR;
    task->registers.ss=KERNEL_DATA_SEGMENT_SELECTOR;
    return 0;

}

struct task * new_task(struct process *process){
    struct task *task=kzalloc(sizeof(struct task));
    int res=0;
    if(!task){
        res=-ENOMEM;
        goto out;
    }
    res=task_initialization(task,process);
    if(!task_head){
        task_head=task;
        task_tail=task;
        current_task=task;
        goto out;
    }
    task_tail->next=task;
    task->prev=task_tail;
    task_tail=task;
out: if(res<0){
      kfree(task);
      return ERROR(res);
     }
     return task;
}