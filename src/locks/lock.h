#ifndef LOCK_H
#define LOCK_H

#include<stdatomic.h>
#include "../task/process.h"
#include "../ipc/sharedmem.h"

#define LOCK_READ 1
#define LOCK_WRITE 2
#define LOCK_UNLOCK 0


struct process;


int lock_acquire(int lock_type,int shm_id,struct process *process);
int lock_release(int lock_type,int shm_id,struct process *process);



struct lock{

    atomic_int lock_type;
    atomic_int reader_count;
    struct process *write_process;
    struct process *read_processes[LOCK_PROCESS_COUNT];
    atomic_int spin_lock;

};


#endif