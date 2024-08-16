#include "process.h"

struct process *current_process;
struct process *processes[NUMBER_OF_TOTAL_PROCESSES]={};

struct process * process_get_process_by_id(uint16_t process_slot) {
    if(process_slot<0 || process_slot>=NUMBER_OF_TOTAL_PROCESSES){
        return -EINVARG;
    }
  
    return processes[process_slot];
}

int process_map_binary(struct process *process){
    return paging_map_to(process->task->chunk,process->phy_addr,(void*)PROGRAM_VIRTUAL_ADDRESS,paging_align_address(process->phy_addr+process->size),PAGING_IS_PREENT|PAGING_IS_WRITABLE|PAGING_ACCESS_FROM_ALL);
}

int process_map_memory(struct process *process){
    return process_map_binary(process);
}

int process_load_binary(char* filename,struct process *process){
    void *program_data_ptr=0;
    int res=0;
    struct disk *disk=disk_get(0);
    int fd=fopen(disk,filename,'r');
    if(!fd){
         res=-EIO;
         goto out;
    }
    struct file_stat *stat;
    res=fstat(fd,stat);
    if(!res){
        res=-EIO;
        goto out;
    }
    program_data_ptr=kzalloc(sizeof(stat->filesize));
    res=fread(program_data_ptr,stat->filesize,1,fd);
    if(!res){
        res=-EIO;
        goto out;
    }
    process->phy_addr=program_data_ptr;
    process->size=stat->filesize;
    process->filetype=PROCESS_BINARY_FILE;

out:if(res<0){
     if(program_data_ptr)
        kfree(program_data_ptr);
    }
    fclose(fd);
    return res;
}

int process_load_data(char *filename,struct process *process){
    return process_load_binary_data(filename,process);
}

int process_load_for_slot(char * filename, struct process **process,uint16_t process_slot){
    struct process *_process;
    int res=0;
    res=process_get_process_by_id(process_slot);
    if(res<=0){
        goto out;
    }

    _process=kzalloc(sizeof(struct process));
    if(!_process){
        res=-ENOMEM;
        goto out;
    }

    void *program_physical_stack=kzalloc(sizeof(USER_PROGRAM_STACK_SIZE));
    if(!program_physical_stack){
        res=-ENOMEM;
        goto out;
        
    }
    
    res=process_load_data(filename,_process);
    if(res<0){
        goto out;
    }

    struct task *task=task_new(_process);
    _process->task=task;
    _process->stack_phy_address=program_physical_stack;
    str_n_cpy(_process->filenmae,filename,sizeof(filename));
    _process->pid=process_slot;

    res=process_map_memory(_process);
    if(res<0){
        goto out;
    }
    *process=_process;
    processes[process_slot]=process;


out: return res;
}

int process_get_free_slot(){
    for(int i=0;i<NUMBER_OF_TOTAL_PROCESSES;i++){
        if(processes[i]==0x0){
            return i;
        }
    }
    return -ENOMEM;
}

int process_load(char *filename,struct process **process){
    int p_slot=process_get_free_slot();
    if(p_slot<0){
        return p_slot;
    }
    return process_load_for_slot(filename,process,p_slot);
}

int process_load_switch(char *filename,struct process** process){
    process_load(filename,process);
}
