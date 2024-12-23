#include "disk.h"

struct disk disk;

void disk_search_and_init(){
    memnset(&disk,0x00,sizeof(disk));
    disk.sector_size=SECTOR_SIZE;
    disk.type=DISK_TYPE_REAL;
    disk.id=0;
    disk.f_system=fs_resolve(&disk);
    
}

struct disk* disk_get(int id){
    if(id!=0)
     return 0;
    return &disk;

}
int disk_read_sectors(int lba,int total,void *buf){
    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xff));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);

    unsigned short* ptr = (unsigned short*) buf;
    for (int b = 0; b < total; b++)
    {
        // Wait for the buffer to be ready
        char c = insb(0x1F7);
        while(!(c & 0x08))
        {
            c = insb(0x1F7);
        }

        // Copy from hard disk to memory
        for (int i = 0; i < 256; i++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }

    }
    return 0;
}

int disk_read_block(struct disk *idisk,int lba,int total,void * buf){
    if(idisk!=&disk){
        return -EIO;
    }
    return disk_read_sectors(lba,total,buf);

}