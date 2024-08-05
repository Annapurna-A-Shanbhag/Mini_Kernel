#ifndef HEAP_H
#define HEAP_H
#include<stdint.h>
#include "../../config.h"
#include "../memory.h"
#include "../../kernel.h"

#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00
#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_IS_FIRST 0b10000000
#define HEAP_BLOCK_HAS_NEXT 0b01000000

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table{
    int table_size;
    HEAP_BLOCK_TABLE_ENTRY* table_entry;
    

};
struct heap{
    void *address;
    struct heap_table *table;
};


void heap_initialization();
void* heap_malloc(struct heap *heap,int size);
void  heap_free(struct heap *heap,void * ptr);

#endif