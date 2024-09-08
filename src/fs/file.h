#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include "../memory/memory.h"
#include "../config.h"
#include "./fat/fat16.h"
#include "./pparser.h"

typedef unsigned int FILE_MODE;

enum
{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

typedef unsigned int FILE_SEEK_MODE;

enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

struct disk;
struct root_path;

typedef void *(*FS_OPEN_FUNCTION)(struct disk *disk, struct root_path *root, FILE_MODE mode);
typedef int (*FS_READ_FUNCTION)(struct disk *disk, void *private, uint32_t size, uint32_t nmemb, char *out);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk *disk);

typedef int (*FS_CLOSE_FUNCTION)(void *private);

typedef int (*FS_SEEK_FUNCTION)(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode);

typedef unsigned int FILE_STAT_FLAGS;

struct file_stat
{
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};
enum
{
    FILE_STAT_READ_ONLY = 0b00000001
};

typedef int (*FS_STAT_FUNCTION)(struct disk *disk, void *private, struct file_stat *stat);

struct filesystem
{
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
    FS_CLOSE_FUNCTION close;
    FS_READ_FUNCTION read;
    FS_SEEK_FUNCTION seek;
    FS_STAT_FUNCTION stat;
    char name[20];
};

struct filesystem_desc
{
    struct filesystem *file;
    int index;     // THe descriptor index
    void *private; // Private data
    struct disk *disk;
};

void fs_initialization();
struct filesystem *fs_resolve(struct disk *disk);
int fopen(char *file_name, char *str_mode);
int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd);
int fseek(int fd, uint32_t offset, FILE_SEEK_MODE mode);
int fstat(int fd, struct file_stat *stat);
int fclose(int fd);

#endif