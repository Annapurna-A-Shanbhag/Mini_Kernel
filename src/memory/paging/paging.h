#ifndef PAGING_H
#define PAGING_H

#include "../heap/kheap.h"
#include "./task/task.h"

#include<stdbool.h>


#define PAGE_SIZE 4096
#define PAGE_DIRECTORY_ENTRIES 1024

#define PAGING_IS_PRESENT 0b00000001
#define PAGING_IS_WRITABLE 0b00000010
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_WRITE_THROUGH 0b00001000
#define PAGING_CACHE_DIABLED 0b00010000

struct paging_4gb_chunk
{
    uint32_t *directory_entry;
};


struct task;
struct paging_4gb_chunk *paging_new_4gb(uint8_t flags);
void paging_switch(struct paging_4gb_chunk *chunk);
void enable_paging();
void load_page();
void *paging_align_address(void *addr);
int paging_map_to(struct paging_4gb_chunk *chunk, void *virt, void *phy, void *end, uint8_t flags);
uint32_t paging_get(uint32_t* directory, void *virt);
int paging_set(uint32_t *directory, void* virt, uint32_t value);
void *paging_convert_virt_to_phy(uint32_t* directory, void *virt);
void *paging_align_to_lower(void *virt);
void paging_free_4gb_chunk(struct paging_4gb_chunk *chunk);
int paging_map(struct paging_4gb_chunk *chunk, void* virt, void* phy, uint8_t flags);

#endif
