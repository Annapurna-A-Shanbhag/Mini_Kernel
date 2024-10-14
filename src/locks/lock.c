#include "lock.h"

void acquire_spin_lock(struct sharedmem *shm){
    int expected=0;
    while(!atomic_compare_exchange_strong(&shm->lock->spin_lock,&expected,0)){
        expected=0;
    }

}

void release_spin_lock(struct sharedmem *shm){
    atomic_store(&shm->lock->spin_lock,0);
}

int lock_assign_process(struct sharedmem *shm,struct process *process){
    for(int i=0;i<LOCK_PROCESS_COUNT;i++){
        if(shm->processes[i]==0){
            shm->processes[i]=process;
            return 0;
        }
    }
    
    return -ENOMEM;

}

int check_for_read_processes(struct sharedmem *shm,struct process *process){
    for(int i=0; i<LOCK_PROCESS_COUNT;i++){
        if(shm->processes[i]==process){
            return 0;
        }
    }
    return -EINVARG;
}


int lock_acquire(int lock_type,int shm_id,struct process *process){
    struct sharedmem *shm= shrdm_find_shmem(shm_id);

    int flag=-1;
    int res=0;
    
    if(lock_type==LOCK_WRITE){

        flag=LOCK_UNLOCK;

        while(!atomic_compare_exchange_strong(&shm->lock->lock_type,&flag,LOCK_WRITE)){
            flag=LOCK_UNLOCK;

        } 
       while(shm->lock->lock_type==LOCK_READ || shm->lock->lock_type==LOCK_WRITE){

       }
        shm->lock->write_process=process;
    }

    else if(lock_type==LOCK_READ){

        
        flag=LOCK_UNLOCK;
        while(!atomic_compare_exchange_strong(&shm->lock->lock_type,&flag,LOCK_READ)){
           flag=LOCK_UNLOCK;
        }

       while(shm->lock->lock_type==LOCK_WRITE){

       }

        res=lock_assign_process(shm,process);
        if(res<0){
          goto out;
       }
       
       shm->lock->reader_count++;

    }
    else{
        res=-EINVARG;
    }
    

out:
    return res;
        
}

int lock_release(int lock_type,int shm_id,struct process *process)
{
    int res=0;
    struct sharedmem *shm= shrdm_find_shmem(shm_id);
    if(!shm){
        res=-EINVARG;
        goto out;
    }

    if(lock_type==LOCK_WRITE){ 

        if(process!=shm->lock->write_process){
            res=-EINVARG;
            goto out;

        }
        shm->lock->lock_type=LOCK_UNLOCK;
        shm->lock->write_process=0;
    }

    else if(lock_type==LOCK_READ){
        
        res=check_for_read_processes(shm,process);
        if(res<0){
            goto out;
        }
        acquire_spin_lock(shm);
        shm->lock->reader_count--;
        if(shm->lock->reader_count==0){
            shm->lock->lock_type=LOCK_UNLOCK;
        }
        release_spin_lock(shm);

    }

    else{
        res=-EINVARG;
    }

out: return res;

}