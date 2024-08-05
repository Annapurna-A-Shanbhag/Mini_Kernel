#include "memory.h"

void memnset(void *ptr,int c,int size){
    char *cptr=(char *)ptr;
    for(int i=0;i<size;i++){
        cptr[i]=(char)c;
    }
}