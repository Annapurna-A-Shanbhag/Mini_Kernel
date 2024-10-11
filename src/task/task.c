#include "task.h"

struct task *task_head = 0;
struct task *task_tail = 0;
struct task *current_task = 0;

struct task *task_current_task()
{
    return current_task;
}
void task_page_task(struct task *task)
{
    user_registers();
    paging_switch(task->chunk);
}

int task_switch(struct task *task)
{
    current_task = task;
    paging_switch(task->chunk);
    return 0;
}

int task_page()
{
    user_registers();
    task_switch(current_task);
    return 0;
}

struct task *task_get_next()
{
    if (!current_task->next)
    {
        return task_head;
    }
    return current_task->next;
}

void task_list_remove(struct task *task)
{
    if (task->prev)
    {
        task->prev->next = task->next;
    }

    if (task_head == task)
    {
        task_head = task->next;
    }

    if (task_tail == task)
    {
        task_tail = task->prev;
    }

    if (task == current_task)
    {
        current_task = task_get_next();
    }
}

void task_free(struct task *task)
{
    paging_free_4gb_chunk(task->chunk);
    task_list_remove(task);
    kfree(task);
    
}

void task_next()
{
    struct task *next_task = task_get_next();
    if (!next_task)
    {
        panic("No more tasks in the system to switch\n");
    }

    task_switch(next_task);
    task_return(&next_task->registers);
}

void *task_get_stack_item(struct task *task, int top)
{
    void *result = 0;
    uint32_t *sp_ptr = (uint32_t*)task->registers.esp;
    task_page_task(task);
    result = (void *)sp_ptr[top];
    kernel_page();
    return result;
}

int copy_string_from_task(struct task *task, void *virt, void *phy, uint32_t size)
{
    int res = 0;
    if (size >= PAGE_SIZE)
    {
        res =-EINVARG;
        goto out;
    }
    char *tmp = kzalloc(size);
    if (!tmp)
    {
        res =-ENOMEM;
        goto out;
    }
    int old_value = paging_get(task->chunk->directory_entry, tmp);
    if (old_value < 0)
    {
        goto out;
    }
    paging_map(task->chunk, tmp,tmp,PAGING_IS_PRESENT | PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL);
    paging_switch(task->chunk);
    str_n_cpy(tmp, virt, size);
    kernel_page();

    res = paging_set(task->chunk->directory_entry, tmp, old_value);
    if (res < 0)
    {
        res = -EIO;
        goto out;
    }
    str_n_cpy(phy, tmp, size);

out:
    if (res < 0)
    {
        if (tmp)
        {
            kfree(tmp);
            
        }
    }
    return res;
}

void task_save_state(struct task *task, struct interrupt_frame *frame)
{
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;
}

void task_current_save_state(struct interrupt_frame *frame){
    struct task *task=task_current_task();
    if(!task){
        panic("No current task to save\n");
    }
    task_save_state(task,frame);
    
}

int task_initialization(struct task *task, struct process *process)
{
    task->chunk = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL); // Why not writable?
    if (!task->chunk)
    {
        return -ENOMEM;
    }
    task->process = process;
    task->registers.ip = PROGRAM_VIRTUAL_ADDRESS;
    if (process->filetype == PROCESS_ELF_FILE)
    {
        task->registers.ip = elf_header(process->elf_file)->e_entry;
    }

    task->registers.esp = PROGRAM_STACK_VIRT_ADDRESS;
    task->registers.cs = USER_CODE_SEGMENT_SELECTOR;
    task->registers.ss = USER_DATA_SEGMENT_SELECTOR;
    return 0;
}

struct task *task_new(struct process *process)
{
    struct task *task = kzalloc(sizeof(struct task));
    int res = 0;
    if (!task)
    {
        res = -ENOMEM;
        goto out;
    }
    res = task_initialization(task, process);
    if (res < 0)
    {
        goto out;
    }
    if (!task_head)
    {
        task_head = task;
        task_tail = task;
        current_task = task;
        goto out;
    }
    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;
out:
    if (ISERR(res))
    {
        if (task){
            kfree(task);
            
        }
        return ERROR(res);
    }
    return task;
}


void task_run_first_ever_task()
{
    if (!current_task)
    {
        panic("task_run_first_ever_task(): No current task exists!\n");
    }

    task_switch(task_head);
    task_return(&task_head->registers);
    //tasks(process->task);
}