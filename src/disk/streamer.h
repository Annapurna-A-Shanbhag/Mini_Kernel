#ifndef STREAMER_H
#define STREAMER_H

#include "disk.h"
#include "../memory/heap/heap.h"
#include "../fs/fat/fat16.h"

struct disk_stream{
    int pos;
    struct disk *disk;

};

struct disk_stream * new_streamer(int id);
int diskstreamer_read( struct disk_stream *read,void* out,size_t total);
int diskstreamer_seek(struct disk_stream *seek,int pos);
void diskstreamer_close(struct disk_stream* stream);

#endif 