#ifndef _STDLIB_H
#define _STDLIB_H
#include <stddef.h>

void *malloc(size_t size);
void free(void *ptr);
char *itoa(int i);
#endif