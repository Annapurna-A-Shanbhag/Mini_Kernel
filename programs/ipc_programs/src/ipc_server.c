#include "../../stdlib/src/stdlib.h"
#include "../../stdlib/src/stdio.h"
#include "../../stdlib/src/api.h"
#include "../../stdlib/src/string.h"


#define SHM_KEY 1024
#define IPC_CREAT 0x01

int main(int arc,char **argv){

    //int write_lock=2;

    print("I am an IPC server!\n");
    uint16_t shm_id= shmget(SHM_KEY,1024,IPC_CREAT);
    if(shm_id==-1){
        print("OOPS! shmget failed\n");
        return 0;
    }

    void* addr=shmat(shm_id);
    if(!addr){
        print("OOPS! shmat failed\n");
        return 0;

    }
    
    //acquire_lock(write_lock,shm_id);
    char str[20]="Hello system !!";
    str_n_cpy((char *)addr," Hello system!! ",sizeof(str));
    //print((char *)addr);

    //release_lock(write_lock,shm_id);

    print("The data loaded in the shared buffer is  ");
    print((char *)addr);


    int res=shmdt((void *)addr);
    if(res==-1){
        print("OOPS! shmdt failed\n");
        return 0;
    }

    while(1){

    }
    
    return 0;

}