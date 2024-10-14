#include "sharedmem.h"

struct sharedmem *sharedmem[IPC_SHRDMEM_MEMORY_REGION_COUNT]={};
uint32_t IPC_VIRT_ADDR[IPC_SHRDMEM_MEMORY_REGION_COUNT]={0x700000,0x701000,0x702000,0x703000,0x704000};


int shardm_free_index(){
    for(int i=0;i<IPC_SHRDMEM_MAX_SIZE;i++){
        if(!sharedmem[i]){
            return i;
        }
    }
    return -EINVARG;
}

int shrdm_assign_process(struct sharedmem *mem,struct process *process)
{
    for(int i=0;i<IPC_SHRDMEM_PROCESS_COUNT;i++){
          if(!mem->processes[i]){
            mem->processes[i]=process;
            return 0;
          }
    }
    return -ENOMEM;
}

struct sharedmem *shrdm_find_shmem(int shm_id){
    for(int i=0;i<IPC_SHRDMEM_MEMORY_REGION_COUNT;i++){
        if(sharedmem[i]->shm_id==shm_id){
            return sharedmem[i];
        }

    }
    return 0;
}


int shrdmget(uint16_t shmkey,size_t size,uint8_t flags){
    int res=0;
    if(size>IPC_SHRDMEM_MAX_SIZE){
        res=-EINVARG;
        goto out;
    }

    struct sharedmem *shrdmem=0;
    int i=0;
    for(i=0;i<IPC_SHRDMEM_MEMORY_REGION_COUNT;i++){
        if(sharedmem[i]->shm_key==shmkey){
            shrdmem=sharedmem[i];
            break;
        }
    }
    if((flags & IPC_CREAT)==0){
        res=-EINVARG;
        goto out;
    }
    struct sharedmem * mem;
    if(shrdmem==0){
        mem=kzalloc(sizeof(struct sharedmem));
        mem->shm_key=shmkey;
        mem->size=size;
        int index= shardm_free_index();
        if(index<0){
            res=index;
            goto out;
        }
        mem->shm_id=index+1;
        
        void *ptr=kzalloc(size);
        mem->phy_addr=ptr;
        mem->virt_addr=(void *)IPC_VIRT_ADDR[index];
        mem->lock->lock_type=LOCK_UNLOCK;
        sharedmem[index]=mem;
        shrdmem=mem;


        res=mem->shm_id;
        
        goto out;

    }

    res=shrdmem->shm_id;
    

out:
    if(res<0){
        if(mem){
            kfree(mem);
        }
    }
    return res;

}

void * shrdmat(uint16_t shmid,struct process *process){

   
    int res=0;
    struct sharedmem *shrdmem=shrdm_find_shmem(shmid);
    if(!shrdmem){
        res=-EINVARG;
        goto out;

    }
    res=shrdm_assign_process(shrdmem,process);
    if(res<0){
        goto out;
    }
    paging_map_to(process->task->chunk,shrdmem->virt_addr,shrdmem->phy_addr,paging_align_address(shrdmem->phy_addr+shrdmem->size),PAGING_IS_PRESENT|PAGING_IS_WRITABLE|PAGING_ACCESS_FROM_ALL);

    return shrdmem->virt_addr;
        
    res=-EINVARG;

out:
   return 0;
    


}

void shrdm_process_unlink(struct sharedmem *mem, struct process *process ){
    for(int i=0;i<IPC_SHRDMEM_PROCESS_COUNT;i++){
        if(mem->processes[i]==process){
            mem->processes[i]=0;
        }
    }
}

int shrdmdt(void *phy_addr,struct process *process){
    
    for(int i=0;i<IPC_SHRDMEM_MEMORY_REGION_COUNT;i++){
        if(sharedmem[i]->phy_addr==phy_addr){
            shrdm_process_unlink(sharedmem[i],process);
            return 0;
            
        }
    }
    return -EINVARG;

}

void shrdm_unlink(uint16_t i){
    sharedmem[i]=0;

}

int shrdmdestroy(void *phy_addr,struct process *process){

    for(int i=0;i<IPC_SHRDMEM_MEMORY_REGION_COUNT;i++){
        if(sharedmem[i]->phy_addr==phy_addr){
            paging_map_to(process->task->chunk,sharedmem[i]->virt_addr,sharedmem[i]->phy_addr,paging_align_address(sharedmem[i]->phy_addr+sharedmem[i]->size),0x00);

            kfree(sharedmem[i]->phy_addr);
            kfree(sharedmem[i]);
            shrdm_unlink(i);
            return 0;
        }
    }
    return -EINVARG;
}