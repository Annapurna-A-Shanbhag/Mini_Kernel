#include "streamer.h"

struct disk_stream *new_streamer(int id)
{
    struct disk *disk = disk_get(id);
    if (!disk)
    {
        return 0;
    }
    struct disk_stream *streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0; // In bytes
    streamer->disk = disk;
    return streamer;
}

int diskstreamer_seek(struct disk_stream *seek, int pos)
{
    seek->pos = pos;
    return 0;
}

int diskstreamer_read(struct disk_stream *read, void *out, size_t total)
{
    int total_to_read = total;
    int res = 0;
    int sector_pos = read->pos / SECTOR_SIZE;
    int offset = read->pos % SECTOR_SIZE;
    char buffer[SECTOR_SIZE];

    if (total > SECTOR_SIZE)
    {
        total_to_read = SECTOR_SIZE - offset; // We are reading these many bytes in this iteration
    }

    total = total - total_to_read;
    res = disk_read_block(read->disk, sector_pos, 1, buffer);

    if (res < 0)
    {
        goto out;
    }

    for (int i = 0; i < total_to_read; i++)
    {
        *(char *)out++ = buffer[offset + i];
    }
    read->pos += total_to_read;
    if (total > 0)
    {
        res = diskstreamer_read(read, out, total);
    }
out:
    return res;
}

void diskstreamer_close(struct disk_stream *stream)
{
    kfree(stream);
}