#include "process.h"

void* isr80h_command6_process_load_start(struct interrupt_frame* frame)
{
    void* filename_user_ptr = task_get_stack_item(task_current_task(), 0);
    char filename[PATH_LIMIT];
    int res = copy_string_from_task(task_current_task(), filename_user_ptr, filename, sizeof(filename));
    if (res < 0)
    {
        goto out;
    }

    char path[PATH_LIMIT];
    str_n_cpy(path, "0:/",3);
    str_n_cpy(path+3, filename,sizeof(filename));

    struct process* process = 0;
    res = process_load_switch(path, &process);
    if (res < 0)
    {
        goto out;
    }

    task_switch(process->task);
    task_return(&process->task->registers);

out:
    return 0;
}
void* isr80h_command7_invoke_system_command(struct interrupt_frame *frame){
   int res=0;
   struct command_arguments *arguments=paging_convert_virt_to_phy(task_current_task()->chunk->directory_entry,task_get_stack_item(task_current_task(),0));
   if(!arguments  || str_len(arguments->argument)<=0){
    return ERROR(-EINVARG);

   }
   struct command_arguments *root_argument=&arguments[0];
   char path[PATH_LIMIT];
   str_n_cpy(path,"0:/",3);
   str_n_cpy(path+3,root_argument->argument,sizeof(arguments->argument));
   
   struct process *process;
   res=process_load_switch(path,&process);
   if(res<0){
    return ERROR(res);

   }
   res=process_inject_arguments(process,root_argument);
   if(res<0){
    return ERROR(res);

   }
   task_switch(process->task);
   task_return(&process->task->registers);
   return 0;

}

void * isr80h_command8_get_program_arguments(struct interrupt_frame *frame){
   
   struct process *process=task_current_task()->process;
   struct process_arguments *arguments=paging_convert_virt_to_phy(task_current_task()->chunk->directory_entry,task_get_stack_item(task_current_task(),0));
   process_get_arguments(process,&arguments->argc,&arguments->argv);
   return 0;

}

void *isr80h_command9_exit(struct interrupt_frame *frame){
    
    struct process *process=task_current_task()->process;
    process_terminate(process);
    return 0;

}