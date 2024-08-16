#include "task.h"

struct task *task_head=0;
struct task *task_tail=0;
struct task *current_task=0;

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