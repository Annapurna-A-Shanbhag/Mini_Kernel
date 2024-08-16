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

void* paging_align_address(void *addr){
    if((uint32_t)addr%PAGE_SIZE==0){
        return addr;
    }
    return (void *)((uint32_t)addr+PAGE_SIZE-((uint32_t)addr%PAGE_SIZE));
}

int paging_get_indexes(uint32_t virt,uint32_t* directory_index,uint32_t* table_index){
    int res = 0;  
    *directory_index = ((uint32_t)virt/ PAGE_DIRECTORY_ENTRIES *PAGE_SIZE);
    *table_index = ((uint32_t)virt % (PAGE_DIRECTORY_ENTRIES * PAGE_SIZE) / PAGE_SIZE);
out:
    return res;
}

int paging_set(uint32_t * directory,uint32_t virt,uint32_t value){
    uint32_t directory_index;
    uint32_t table_index;
    int res=0;
    res=paging_get_indexes(virt,&directory_index,&table_index);
    if(res<0){
        return res;
    }
    uint32_t entry=directory[directory_index];
    uint32_t *page_table=(uint32_t*)(entry|0xfffff000);
    page_table[table_index]=value;
    return res;

}

int paging_map(struct paging_4gb_chunk *chunk,uint32_t phy,uint32_t virt,uint8_t flags){
    if(phy%PAGE_SIZE!=0 || virt%PAGE_SIZE!=0){
        return -EINVARG;
    }
    return paging_set(chunk->directory_entry,virt,phy|flags);

}
int paging_map_range(struct paging_4gb_chunk* chunk,uint32_t phy,uint32_t virt,uint32_t pages,uint8_t flags){
    int res=0;
    for(int i=0;i<pages;i++){
        res=paging_map(chunk,phy,virt,flags);
        phy+=PAGE_SIZE;
        virt+=PAGE_SIZE; 
    }
    return res;

}

int paging_map_to(struct paing_4gb_chunk *chunk,void*phy,void*virt,void* end,uint8_t flags){
    int res=0;
    if((uint32_t)phy%PAGE_SIZE!=0){
        res=-EINVARG;
        goto out;
    }
    if((uint32_t)virt%PAGE_SIZE!=0){
        res=-EINVARG;
        goto out;
    }
    if((uint32_t)end%PAGE_SIZE!=0){
        res=-EINVARG;
        goto out;
    }
    if((uint32_t)end<(uint32_t)phy){
        res=-EINVARG;
        goto out;
    }
    uint32_t size=(uint32_t)end-(uint32_t)phy;
    uint32_t pages=size/PAGE_SIZE;
    res=paging_map_range(chunk,(uint32_t)phy,(uint32_t)virt,pages,flags);
out:return res;
}

