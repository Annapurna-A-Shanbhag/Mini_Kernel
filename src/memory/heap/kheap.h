#ifndef KHEAP_H
#define KHEAP_H
#include "heap.h"

void kheap_initialization();
void *kzalloc(size_t size);
int kfree(void *ptr);

#endif
