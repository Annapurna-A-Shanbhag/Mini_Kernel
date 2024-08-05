#include "heap.h"

int heap_validate_address_alignment(void* addr){
    return (uint32_t)addr%(uint32_t)HEAP_BLOCK_SIZE;
}

void heap_table_initialization(struct heap_table *table,int total){
    void * addr=table->table_entry;
    memnset(addr,HEAP_BLOCK_TABLE_ENTRY_FREE,total);
}

void heap_initialization(struct heap heap,struct heap_table table,void* end_addr,int total){       
    memnset(&heap,0,sizeof(heap));
    heap.address=(void *)HEAP_ADDRESS;
    heap.table =&table;
    if(heap_validate_address_alignment(heap.address) || heap_validate_address_alignment(end_addr)){
        print("Validation failed\n");
        return;
    }

    heap_table_initialization(&table,total);

}

int heap_convert_bytes_to_blocks(int size){
    int blocks=size/HEAP_BLOCK_SIZE;
    int remainder=size%HEAP_BLOCK_SIZE;
    if(remainder!=0){
        blocks++;
    }
    return blocks;

}

int heap_find_first_free_block(struct heap *heap,int blocks){
    struct heap_table *table=heap->table;
    int no_of_blocks=0;
    int index=0;
    for(int i=0;i<table->table_size;i++){
        if(no_of_blocks==0){
            index=i;
        }
        if(table->table_entry[i]==0x00){
          no_of_blocks++;
          if(blocks==no_of_blocks){
            return index;
          }
        }
        else{
            no_of_blocks=0;
        }
    }
    print("No memory left\n");
    return -1;
}

void* heap_convert_block_to_address(struct heap *heap,int block){
    return heap->address+block*HEAP_BLOCK_SIZE;
}

void heap_mark_table_entries_as_taken(struct heap *heap,int first_block,int size){
    struct heap_table *table=heap->table;
    int end_block=first_block+size;
    HEAP_BLOCK_TABLE_ENTRY *addr=table->table_entry;
    addr[first_block]=HEAP_BLOCK_IS_FIRST;
    for(int i=first_block;i<end_block;i++){
       addr[i]|=HEAP_BLOCK_TABLE_ENTRY_TAKEN;
       if(i+1!=end_block){
        addr[i]|=HEAP_BLOCK_HAS_NEXT;
       }
    }
}

void* heap_malloc(struct heap *heap,int size){
    int blocks=heap_convert_bytes_to_blocks(size);
    int first_block=heap_find_first_free_block(heap,blocks);
    void * addr=heap_convert_block_to_address(heap,first_block);
    heap_mark_table_entries_as_taken(heap,first_block,blocks);
    return addr;
}

int heap_convert_address_to_blocks(struct heap *heap,void * ptr){
    int blocks=(uint32_t)ptr%HEAP_BLOCK_SIZE;
    return blocks;
}

int heap_mark_table_entries_as_free(struct heap *heap,int first_block){
    HEAP_BLOCK_TABLE_ENTRY* addr=heap->table->table_entry;
    if((addr[first_block]&0x80)!=0x80){
       print("Not the first block\n"); 
       return -1;
    }
      
    int i=0;
    while((addr[i]& HEAP_BLOCK_TABLE_ENTRY_TAKEN)!=0x0){
        addr[i]=HEAP_BLOCK_TABLE_ENTRY_FREE;
        i++;
    }
    addr[i]= HEAP_BLOCK_TABLE_ENTRY_FREE;
    return i;

}

int heap_convert_blocks_to_bytes(int blocks){
    return blocks*HEAP_BLOCK_SIZE;
}
void heap_mark_heap_entries_as_free(struct heap * heap,int size,void *ptr){
    memnset(ptr,0,size);
}

void  heap_free(struct heap *heap,void * ptr){
    int res=heap_validate_address_alignment(ptr);
    if(res!=0)
      return;
    int start_block=heap_convert_address_to_blocks(heap,ptr);
    int blocks=heap_mark_table_entries_as_free(heap,start_block);
    int bytes=heap_convert_blocks_to_bytes(blocks);
    heap_mark_heap_entries_as_free(heap,bytes,ptr);

}