#ifndef PATHPARSER_H
#define PATHPARSER_H

#include "../config.h"
#include "../string/string.h"
#include "../disk/disk.h"
#include "../memory/heap/kheap.h"


struct disk;

struct path_part{
    char *name;
    struct path_part *next;
};
struct root_path{
    int disk_id;
    struct  path_part *part;

};

struct root_path * path_parser(struct disk *disk,char* file_path);


#endif