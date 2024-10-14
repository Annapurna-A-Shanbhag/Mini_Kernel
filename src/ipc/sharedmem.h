#ifndef SHAREDMEM_H
#define SHAREDMEM_H

#include<stddef.h>
#include<stdint.h>
#include "../task/process.h"
#include "../memory/heap/kheap.h"
#include "../memory/paging/paging.h"
#include "../locks/lock.h"

#define IPC_CREAT 0x01



struct sharedmem{
    uint16_t shm_key;
    uint16_t shm_id;
    void* virt_addr;
    void* phy_addr;
    struct process *processes[IPC_SHRDMEM_PROCESS_COUNT];
    size_t size;
    struct lock *lock;
};

int shrdmget(uint16_t shmkey,size_t size,uint8_t flags);
void* shrdmat(uint16_t shmid,struct process *process);
int shrdmdt(void *virt_addr,struct process *process);
int shrdmdestroy(void *virt_addr,struct process *process);
struct sharedmem *shrdm_find_shmem(int shm_id);


#endif