#ifndef MEMORY_H
#define MEMORY_H
#include <stddef.h>
void memnset(void *ptr,int c,size_t size);
void* memncpy(void *dest,void* src,size_t size);

#endif