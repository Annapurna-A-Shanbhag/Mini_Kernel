#include "memory.h"

void memnset(void *ptr,int c,size_t size){
    char *cptr=(char *)ptr;
    for(int i=0;i<size;i++){
        cptr[i]=(char)c;
    }
}

void* memncpy(void *dest,void* src,size_t size){
    char *d=dest;
    char *s=src;
    for(int i=0;i<size;i++){
        *d++ = *s++;
    }
    return dest;
}