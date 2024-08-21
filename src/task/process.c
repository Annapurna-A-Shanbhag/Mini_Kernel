#include "process.h"

struct process *current_process;
struct process *processes[NUMBER_OF_TOTAL_PROCESSES]={};
struct process *current_process=0;

void process_get_arguments(struct process *process,int *argc, char ***argv){

    argc=process->arguments.argc;
    argv=process->arguments.argv;

}

int process_get_number_of_arguments(struct command_arguments *root_argument){
    struct command_arguments *current=root_argument;
    int count=0;
    while(current){
        current=current->next;
        count++;
    }
    return count;

}

int process_inject_arguments(struct process *process,struct command_arguments *root_argument){
    int res=0;
    int argc=process_get_number_of_arguments(root_argument);
    if(argc==0){
        res=-EINVARG;
        goto out;

    }
    char **argv=process_malloc(process,argc*sizeof(char *));
    struct command_arguments *current=root_argument;
    int i=0;
    while(current){
        char *arg=process_malloc(process,sizeof(current->argument));
        str_n_cpy(arg,current->argument,sizeof(current->argument));
        argv[i]=arg;
        current=current->next;
        i++;

   }
   process->arguments.argc=argc;
   process->arguments.argv=argv;
out: return res;  
}

struct process * process_get_process_by_id(uint16_t process_slot) {
    if(process_slot<0 || process_slot>=NUMBER_OF_TOTAL_PROCESSES){
        return -EINVARG;
    }
  
    return processes[process_slot];
}

struct process_allocation *process_get_alloc_index(struct process *process){
    for(int i=0;i<NUMBER_OF_PROCESS_ALLOCATION;i++){
        if(process->allocations[i].addr==0){
            return &process->allocations[i];
        }
    }
    return 0;
}


int process_malloc(struct process *process,size_t size){
    int res=0;
    struct process_allocation *allocation=process_get_free_alloc_index(process);
    if(allocation==0){
        res=-ENOMEM;
        goto out;
    }
    void *phy=kzalloc(sizeof(size));
    if(!phy){
        res=-ENOMEM;
        goto out;
    }
    res=paging_map_to(process->task->chunk,phy,phy,paging_align_address((uint32_t)phy+size),PAGING_IS_PREENT|PAGING_IS_WRITABLE|PAGING_ACCESS_FROM_ALL);
    if(res<0){
        res=-EIO;
        goto out;
    }
    allocation->addr=phy;
    allocation->size=size;

out:if(res<0){
      kfree(phy);
    }
    return res;

}

struct process_allocation * process_get_alloc_by_addr(struct process *process,void* ptr){
    for(int i=0;i<NUMBER_OF_PROCESS_ALLOCATION;i++){
        if(process->allocations[i].addr==ptr){
            return &process->allocations[i];
        }
    }
    return -EIO;

}

void process_allocation_unjoin(struct process_allocation *allocation){
    allocation->addr=0;
    allocation->size=0;

}

int process_free(struct process *process,void* ptr){
    int res=0;
    struct process_allocation * allocation=process_get_alloc_by_addr(process,ptr);
    if(!allocation){
        res=-EIO;
        goto out;
    }
    res=paging_map_to(process->task->chunk,ptr,ptr,paging_align_address((uint32_t)allocation->addr+allocation->size),000);
    if(res<0){
        res-EIO;
        goto out;
    }
    process_allocation_unjoin(allocation);
    kfree(ptr);

out:return res;
}

void process_switch(struct process *process){
    current_process=process;
}

void process_switch_to_any(){
    for(int i=0;i<NUMBER_OF_TOTAL_PROCESSES;i++){
        if(processes[i]){
            process_switch(process[i]);
        }
    }
}

void process_unlink(struct process *process){
    processes[process->pid]=0x00;
    if(current_process==process){
        process_switch_to_any();
    }

}

int process_free_allocations(struct process *process){
    for(int i=0;i<NUMBER_OF_PROCESS_ALLOCATION;i++){
      process_free(process,process->allocations[i].ptr);
    }
    return 0;
}

void process_free_binary_data(struct process *process){
    kfree(process->phy_addr);
}

void process_free_program_data(struct process *process){
    process_free_binary_data(process);
}

int process_terminate(struct process *process){
    int res=0;
    res=process_free_allocations(process);
    process_free_program_data(process);
    kfree(process->stack_phy_address);
    task_free(process->task);
    process_unlink(process);
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
