#include "file.h"

struct filesystem *filesystems[NUMBER_OF_FILESYTEMS];
struct filesystem_desc *filesystem_descs[NUMBER_OF_FILE_DESCRIPTORS];

FILE_MODE file_get_mode_by_string(char *str)
{
    FILE_MODE mode = FILE_MODE_INVALID;
    if (str_n_cmp(str, "r", 1) == 0)
    {
        mode = FILE_MODE_READ;
    }
    else if (str_n_cmp(str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }
    else if (str_n_cmp(str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }
    return mode;
}

int new_descriptor(struct filesystem_desc **desc)
{
    struct filesystem_desc *descriptor = 0;
    int res = 0;
    descriptor = kzalloc(sizeof(struct filesystem_desc));
    if (!descriptor)
    {
        res = -ENOMEM;
        goto out;
    }
    for (int i = 0; i < NUMBER_OF_FILE_DESCRIPTORS; i++)
    {
        if (filesystem_descs[i] == 0x00)
        {
            descriptor->index = i + 1;
            *desc = descriptor;
            break;
        }
    }

out:
    return res;
}

struct filesystem_desc *fs_get_fs_descriptor(int fd)
{
    if (fd <= 0 || fd > NUMBER_OF_FILE_DESCRIPTORS)
    {
        return 0;
    }

    return filesystem_descs[fd - 1]; // Made changes here
}

void fs_descriptor_initialization(struct filesystem_desc **filesystem_descs)
{
    memnset(filesystem_descs, 0, sizeof(filesystem_descs));
}

struct filesystem **fs_get_free_filesystem(struct filesystem *f_system)
{
    struct filesystem **fs = 0;
    for (int i = 0; i < NUMBER_OF_FILESYTEMS; i++)
    {
        if (filesystems[i] == 0x0)
        {
            return &filesystems[i]; // Changes made here
        }
    }
    return fs;
}

void fs_insert_filesystem(struct filesystem *f_system)
{
    struct filesystem **fs;
    fs = fs_get_free_filesystem(f_system);
    if (!fs)
        return;
    *fs = f_system;
}

void fs_initialization()
{
    
    memnset(filesystems, 0, sizeof(filesystems));
    fs_descriptor_initialization(filesystem_descs);
    struct filesystem *f_16 = fat16_initialization();
    fs_insert_filesystem(f_16);
}

struct filesystem *fs_resolve(struct disk *disk)
{
    struct filesystem *f_system = 0;
    for (int i = 0; i < NUMBER_OF_FILESYTEMS; i++)
    {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
        {
            f_system = filesystems[i];
            return f_system;
        }
    }
    return f_system;
}

int fopen(char *file_name, char *str_mode)
{
    int res = 0;
    FILE_MODE mode = file_get_mode_by_string(str_mode);
    if (mode == FILE_MODE_INVALID)
    {
        res = -EINVARG;
        goto out;
    }
    struct root_path *root = path_parser(file_name);
    if (!root)
    {
        res = -EINVARG;
        goto out;
    }
    if (!root->part)
    {
        res = -EINVARG;
        goto out;
    }
    struct disk *disk = disk_get(root->disk_id);
    if (!disk)
    {
        res = -EIO;
        goto out;
    }
    if (!disk->f_system)
    {
        res = -EIO;
        goto out;
    }
    void *descriptor_private = disk->f_system->open(disk, root, mode);
    
    if(ISERR(descriptor_private)){
        res=ERROR_I(descriptor_private);
        goto out;

    }
    struct filesystem_desc *desc;
    int response = new_descriptor(&desc);
    if (response<0)
    {
        res = response;
        goto out;
    }
    desc->disk = disk;
    desc->file = disk->f_system;
    desc->private = descriptor_private;
    filesystem_descs[desc->index-1]=desc;
    res = desc->index;

out:
    if (res < 0)
        return 0;
    return res;
}

int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd)
{
    int res = 0;
    if (fd < 0 || size == 0 || nmemb == 0)
    {
        res = -EINVARG;
        goto out;
    }
    struct filesystem_desc *desc = 0;
    desc = fs_get_fs_descriptor(fd);
    if (!desc)
    {
        res = -EINVARG;
        goto out;
    }
    struct disk *disk = desc->disk;
    void *private = desc->private;
    res = desc->file->read(disk, private, size, nmemb, ptr);
out:
    return res;
}

int fseek(int fd, uint32_t offset, FILE_SEEK_MODE mode)
{
    int res = 0;
    struct filesystem_desc *desc = 0;
    desc = fs_get_fs_descriptor(fd);
    if (!desc)
    {
        res = -EINVARG;
        goto out;
    }
    res = desc->file->seek(desc->private, offset, mode);
out:
    return res;
}

int fstat(int fd, struct file_stat *stat)
{
    int res = 0;
    struct filesystem_desc *desc = 0;
    desc = fs_get_fs_descriptor(fd);
    if (!desc)
    {
        res = -EINVARG;
        goto out;
    }
    res = desc->file->stat(desc->disk, desc->private, stat);
out:
    return res;
}

void file_free_descriptor(struct filesystem_desc *desc)
{
    filesystem_descs[desc->index - 1] = 0x00;
    kfree(desc);
}

int fclose(int fd)
{
    int res = 0;
    struct filesystem_desc *desc = 0;
    desc = fs_get_fs_descriptor(fd); // pathparser_free??
    if (!desc)
    {
        res = -EINVARG;
        goto out;
    }
    res = desc->file->close(desc->private);
    if (res == ALL_OK)
        file_free_descriptor(desc);
out:
    return res;
}