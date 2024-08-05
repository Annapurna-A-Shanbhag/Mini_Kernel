#include "kheap.h"
#include "heap.h"


struct heap_table kernel_table;
struct heap kernel_heap;

void kheap_initialization(){
    memnset(&kernel_table,0,sizeof(kernel_table));

    kernel_table.table_entry=(HEAP_BLOCK_TABLE_ENTRY *)HEAP_TABLE_ADDRESS;
    kernel_table.table_size=(int)(HEAP_SIZE/HEAP_BLOCK_SIZE);
    int total=sizeof(HEAP_BLOCK_TABLE_ENTRY)*kernel_table.table_size;
    uint32_t end_addr=((uint32_t)HEAP_ADDRESS+(uint32_t)HEAP_SIZE);
    
    heap_initialization(kernel_heap,kernel_table,end_addr,total);

}

void* kmalloc(int size){

   void* ptr=heap_malloc(&kernel_heap,size);
   return ptr;

}

void* kzalloc(int size){
    void *ptr=kmalloc(size);
    memnset(ptr,0,size);
    return ptr;
}

void kfree(void * ptr){
    heap_free(&kernel_heap,ptr);
}