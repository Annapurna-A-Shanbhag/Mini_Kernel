#include "../../stdlib/src/stdlib.h"
#include "../../stdlib/src/stdio.h"
#include "../../stdlib/src/api.h"


#define SHM_KEY 1024
#define IPC_CREAT 0x01

int main(int arc,char **argv){

    print("\nI am an IPC client!\n");

    uint16_t shm_id= shmget(SHM_KEY,1024,IPC_CREAT);
    if(shm_id==-1){
        print("OOPS! shmget failed\n");
        return 0;
    }
    void *addr=shmat(shm_id);
     if(!addr){
        print("OOPS! shmat failed\n");
        return 0;
    }

    print("The data stored in the shared buffer is  ");    
    //acquire_lock(1,shm_id);
    print((char *)addr);
    //release_lock(1,shm_id);

    int res=shmdt((void *)addr);
    if(res==-1){
        print("OOPS! shmdt failed\n");
        return 0;
    }

    res=shmdestroy((void *)addr);
    if(res<0){
        print("OOPS! shmdestroy failed\n");
    }
    
    while(1){
        
    } 

    return 0;

}