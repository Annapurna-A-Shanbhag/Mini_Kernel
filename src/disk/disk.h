#ifndef DISK_H
#define DISK_H

#include "../fs/file.h"
#include"../config.h"
#include "../io/io.h"

typedef unsigned int DISK_TYPE;

#define DISK_TYPE_REAL 0;


struct disk{
    int id;
    struct filesystem *f_system;
    DISK_TYPE type;
    void* fs_private;
    int sector_size;

};
void disk_search_and_init();
struct disk* disk_get(int id);
int disk_read_block(struct disk *idisk,int lba,int total,void* buf);



#endif