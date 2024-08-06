#include "paging.h"

struct paging_4gb_chunk * current_directory;
struct paging_4gb_chunk * paging_new_4gb(uint8_t flags){
    uint32_t * directory=kzalloc(sizeof(uint32_t)*PAGE_DIRECTORY_ENTRIES);
    uint32_t index=0;

    for(int i=0;i<PAGE_DIRECTORY_ENTRIES;i++){
        uint32_t *page_table=kzalloc(sizeof(uint32_t)*PAGE_DIRECTORY_ENTRIES);
        for(int j=0;j<PAGE_DIRECTORY_ENTRIES;j++){
            page_table[j]= (index+j*PAGE_SIZE) | flags;
            
        }
        index+=PAGE_DIRECTORY_ENTRIES * PAGE_SIZE;
        directory[i]=(uint32_t)page_table | flags |PAGING_IS_WRITABLE ;
    }
    
    struct paging_4gb_chunk *chunk =kzalloc(sizeof(struct paging_4gb_chunk));
    chunk->directory_entry=directory;
    return chunk;
    
}

void paging_switch(struct paging_4gb_chunk *chunk){
    load_page(chunk);
    current_directory=chunk;

}

