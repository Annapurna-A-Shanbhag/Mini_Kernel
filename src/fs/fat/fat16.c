#include "fat16.h"
#include "../../kernel.h"

#define FAT16_SIGNATURE 0x29
#define FAT16_FAT_ENTRY_SIZE 0x02
#define FAT16_BAD_SECTOR 0xFF7
#define FAT16_UNUSED 0x00

typedef unsigned int FAT_ITEM_TYPE;
#define FAT16_ITEM_TYPE_DIRECTORY 0
#define FAT16_ITEM_TYPE_FILE 1

// Fat directory entry attributes bitmask
#define FAT16_FILE_READ_ONLY 0x01
#define FAT16_FILE_HIDDEN 0x02
#define FAT16_FILE_SYSTEM 0x04
#define FAT16_FILE_VOLUME_LABEL 0x08
#define FAT16_FILE_SUBDIRECTORY 0x10
#define FAT16_FILE_ARCHIVED 0x20
#define FAT16_FILE_DEVICE 0x40
#define FAT16_FILE_RESERVED 0x80

typedef unsigned int FAT_ITEM_TYPE;

struct fat_header
{
    uint8_t BS_jmp_boot[3];
    uint8_t BS_oem_name[8];
    uint16_t BPB_bytes_per_sec;
    uint8_t BPB_sec_per_cluster;
    uint16_t BPB_rsvd_sec_count;
    uint8_t BPB_number_fats;
    uint16_t BPB_root_entry_count;
    uint16_t BPB_total_sectors16;
    uint8_t BPB_media;
    uint16_t BPB_fat_size16;
    uint16_t BPB_sectors_per_track;
    uint16_t BPB_number_heads;
    uint32_t BPB_hidden_sectors;
    uint32_t BPB_total_sectors32;

} __attribute__((packed));

struct fat_header_extended
{
    uint8_t BS_drive_number;
    uint8_t BS_reserved;
    uint8_t BS_boot_signature;
    uint32_t BS_volume_id;
    uint8_t BS_volume_label[11];
    uint8_t BS_file_sys_type[8];

} __attribute__((packed));

struct fat_h
{
    struct fat_header primary_header;
    union fat_h_e
    {
        struct fat_header_extended extended_header;
    } shared;
};

struct fat_directory_item
{
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t low_16_bits_first_cluster;
    uint32_t filesize;
} __attribute__((packed));

struct fat_directory
{
    struct fat_directory_item *fat_item;
    int total;
    int starting_sector_pos;
    int ending_sector_pos;
};

struct fat_item
{
    union
    {
        struct fat_directory *directory;
        struct fat_directory_item *item;
    };
    FAT_ITEM_TYPE item_type;
};

struct fat_private
{
    struct fat_h header;
    struct fat_directory root_directory;
    struct disk_stream *cluster_read_stream;
    struct disk_stream *fat_read_stream;
    struct disk_stream *directory_stream;
};

int fat16_resolve(struct disk *disk);
void *fat16_open(struct disk *disk, struct root_path *root, FILE_MODE mode);
int fat16_read(struct disk *disk, void *desc, uint32_t size, uint32_t nmemb, char *out_ptr);
int fat16_seek(void *desc, uint32_t offset, FILE_SEEK_MODE mode);
int fat16_stat(struct disk *disk, void *desc, struct file_stat *stat);
int fat16_close(void *desc);

struct filesystem fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open,
    .seek = fat16_seek,
    .close = fat16_close,
    .stat = fat16_stat,
    .read = fat16_read};

struct fat16_fs_desc
{
    int pos;
    struct fat_item *item;
};

void private_initialization(struct disk *disk, struct fat_private *private)
{
    private->cluster_read_stream = new_streamer(disk->id);
    private->directory_stream = new_streamer(disk->id);
    private->fat_read_stream = new_streamer(disk->id);
}
struct filesystem *fat16_initialization()
{
    str_n_cpy(fat16_fs.name, "FAT_16", sizeof(fat16_fs.name));
    return &fat16_fs;
}

int fat16_get_total_items_for_directory(uint32_t start_sector, struct fat_private *private)
{
    int res = 0;
    struct fat_directory_item item;
    int total_bytes_from_beginning = start_sector * private->header.primary_header.BPB_bytes_per_sec;
    struct disk_stream *directory_stream = private->directory_stream;
    res = diskstreamer_seek(directory_stream, total_bytes_from_beginning);

    if (res < 0)
    {
        res = -EIO;
        goto out;
    }

    int count = 0;
    while (1)
    {
        if (diskstreamer_read(directory_stream, &item, sizeof(item)) != ALL_OK)
        {
            res = -EIO;
            goto out;
        }
        if (item.filename[0] == 0x0)
            break;
        if (item.filename[0] == 0xE5)
        {
            continue;
        }
        count++;
    }
    res = count;

out:
    return res;
}

int fat16_get_root_entries(struct disk *disk, struct fat_private *private, struct fat_directory *directory)
{
    int res = 0;
    struct fat_header *header = &private->header.primary_header;
    uint32_t root_directory_start_sector = header->BPB_fat_size16 * header->BPB_number_fats + header->BPB_hidden_sectors;
    uint32_t root_directory_size = header->BPB_root_entry_count * sizeof(struct fat_directory_item);
    uint32_t root_directory_size_sectors = root_directory_size / header->BPB_bytes_per_sec;
    uint32_t root_directory_end_sector = root_directory_start_sector + root_directory_size_sectors;

    if (root_directory_size % header->BPB_bytes_per_sec != 0)
    {
        root_directory_size_sectors++;
    }
    int count = fat16_get_total_items_for_directory(root_directory_start_sector, private);
    if (count < 0)
    {
        res = count;
        goto out;
    }
    struct fat_directory_item *directory_item = kzalloc(root_directory_size); // Changes made here.

    if (!directory_item)
    {
        res = -ENOMEM;
        goto out;
    }
    struct disk_stream *stream = private->directory_stream;
    int total_bytes_from_beginning = root_directory_start_sector * private->header.primary_header.BPB_bytes_per_sec;

    if (diskstreamer_seek(stream, total_bytes_from_beginning) != ALL_OK)
    {
        res = -EIO;
        goto out;
    }
    if (diskstreamer_read(stream, directory_item, root_directory_size) != ALL_OK)
    {
        res = -EIO;
        goto out;
    }

    directory->fat_item = directory_item;
    directory->total = count;
    directory->starting_sector_pos = root_directory_start_sector;
    directory->ending_sector_pos = root_directory_end_sector;

out:
    if (!directory_item)
    {
        kfree(directory_item);
    }
    return res;
}

void kfree_directory(struct fat_directory *directory)
{
    if (!directory)
    {
        return;
    }
    if (directory->fat_item)
    {
        kfree(directory->fat_item);
    }

    kfree(directory);
}

void fat16_free_fat_item(struct fat_item *f_item)
{

    if (f_item->item_type == FAT16_ITEM_TYPE_FILE)
    {
        kfree(f_item->item);
    }
    if (f_item->item_type == FAT16_ITEM_TYPE_DIRECTORY)
    {
        kfree_directory(f_item->directory);
    }
    kfree(f_item);
}

void fat16_free_fat_desc(struct fat16_fs_desc *desc)
{
    fat16_free_fat_item(desc->item);
    kfree(desc);
}

int fat16_resolve(struct disk *disk)
{
    int res = 0;
    struct fat_private *private = kzalloc(sizeof(struct fat_private)); // Need to make test for private.
    private_initialization(disk, private);
    disk->f_system = &fat16_fs;
    disk->fs_private = private;

    struct disk_stream *read_stream = new_streamer(disk->id);
    if (!read_stream)
    {
        res = -ENOMEM;
        goto out;
    }
    if (diskstreamer_read(read_stream, &private->header, sizeof(struct fat_header)) != ALL_OK)
    {
        res = -EIO;
        goto out;
    }
    if (private->header.shared.extended_header.BS_boot_signature != 0x29)
    {
        res = -EFSNOTUS;
        goto out;
    }

    if (fat16_get_root_entries(disk, private, &private->root_directory) != ALL_OK)
    {
        res = -EIO;
        goto out;
    }

out:
    if (read_stream) // Made changes here
    {
        diskstreamer_close(read_stream);
    }
    if (res < 0)
    {
        kfree(private);
        disk->fs_private = 0;
    }
    return res;
}

void fat16_to_proper_string(char **out, unsigned char *in, size_t size)
{
    int i = 0;
    while (*in != 0x00 && *in != 0x20)
    {
        **out = *in;
        *in += 1;
        *out += 1;
        if (i >= size - 1)
        {
            break;
        }
        i++;
    }
    **out = 0x0;
}

void fat16_get_full_relative_filename(struct fat_directory_item *item, char *out, size_t size)
{
    memnset(out, 0, sizeof(out));
    char *tmp_out = out;
    fat16_to_proper_string(&tmp_out, item->filename, sizeof(item->filename));
    if (item->ext[0] != 0x0 && item->ext[0] != 0x20)
    {
        *tmp_out++ = '.';
        fat16_to_proper_string(&tmp_out, (unsigned char *)item->ext, sizeof(item->ext));
    }
}
uint16_t get_first_cluster_of_directory(struct fat_directory_item *item)
{
    return (item->high_16_bits_first_cluster | item->low_16_bits_first_cluster);
}
int convert_cluster_to_sector(uint16_t cluster, struct fat_private *private)
{
    return (private->root_directory.ending_sector_pos + (cluster - 2) * private->header.primary_header.BPB_sec_per_cluster);
}

int fat16_get_fat_entry(int cluster, int offset, struct fat_private *private)
{
    int res = 0;
    int result = 0;
    int starting_fat_table_pos = private->header.primary_header.BPB_rsvd_sec_count * private->header.primary_header.BPB_bytes_per_sec;
    struct disk_stream *stream = private->fat_read_stream;
    if (!stream) // Added this.
    {
        res - ENOMEM;
        goto out;
    }
    if (diskstreamer_seek(stream, starting_fat_table_pos + (cluster * FAT16_FAT_ENTRY_SIZE)) != ALL_OK) // Made changes here
    {
        res = -EIO;
        goto out;
    }

    if (diskstreamer_read(stream, &result, sizeof(result)) != ALL_OK)
    {
        res = -EIO;
        goto out;
    }
    res = result;

out:
    return res;
}

int fat16_get_cluster_for_offset(uint16_t cluster, int offset, struct fat_private *private)
{
    uint16_t one_cluster_to_bytes = private->header.primary_header.BPB_sec_per_cluster * private->header.primary_header.BPB_bytes_per_sec;
    uint16_t cluster_to_use = cluster;
    uint16_t clusters_ahead = offset / one_cluster_to_bytes; // Made changes here
    int res = 0;
    for (int i = 0; i < clusters_ahead; i++)
    {
        int entry = fat16_get_fat_entry(cluster_to_use, offset, private);
        if (entry == 0xFF8 || entry == 0xFFF)
        {
            // We are at the last entry in the file
            res = -EIO;
            goto out;
        }

        // Sector is marked as bad?
        if (entry == FAT16_BAD_SECTOR)
        {
            res = -EIO;
            goto out;
        }

        // Reserved sector?
        if (entry == 0xFF0 || entry == 0xFF6)
        {
            res = -EIO;
            goto out;
        }

        if (entry == 0x00)
        {
            res = -EIO;
            goto out;
        }

        cluster_to_use = entry;
    }
    res = cluster_to_use; // Made changes here
out:
    return res;
}

int fat16_read_internal_stream(struct disk_stream *stream, uint16_t cluster, int offset, int size, struct fat_private *private, void *item)
{
    int res = 0;
    uint16_t one_cluster_to_bytes = private->header.primary_header.BPB_sec_per_cluster * private->header.primary_header.BPB_bytes_per_sec;
    uint16_t cluster_to_use = fat16_get_cluster_for_offset(cluster, offset, private);

    if (cluster_to_use < 0)
    {
        res = cluster_to_use;
        goto out;
    }

    int starting_sector = convert_cluster_to_sector(cluster_to_use, private);
    int offset_from_cluster = offset % one_cluster_to_bytes;
    int cluster_starting_pos = starting_sector * private->header.primary_header.BPB_bytes_per_sec + offset_from_cluster;
    int total_to_read = size > one_cluster_to_bytes ? one_cluster_to_bytes : size;

    if (diskstreamer_seek(stream, cluster_starting_pos) != ALL_OK)
    {
        res = -EIO;
        goto out;
    }
    if (diskstreamer_read(stream, item, total_to_read) != ALL_OK)
    {
        res = -EIO;
        goto out;
    }
    size -= total_to_read;
    if (size > 0)
    {
        res = fat16_read_internal_stream(stream, cluster, offset + total_to_read, size, private, item + total_to_read); // Made changes here
    }
out:
    return res;
}
int fat16_read_internal(uint16_t cluster, int offset, uint32_t directory_size, struct fat_private *private, void *item)
{
    struct disk_stream *stream = private->cluster_read_stream;
    return fat16_read_internal_stream(stream, cluster, offset, directory_size, private, item);
}

struct fat_directory *fat16_load_directory(struct fat_directory_item *item, struct fat_private *private) // Few changes have to be made in the original code
{
    int res;
    if (!(item->attribute & FAT16_FILE_SUBDIRECTORY))
    {
        res = -EIO;
        goto out;
    }

    struct fat_directory *directory = kzalloc(sizeof(struct fat_directory));
    if (!directory)
    {
        res = -ENOMEM;
        goto out;
    }

    uint16_t first_cluster = get_first_cluster_of_directory(item);
    int starting_sector_of_directory = convert_cluster_to_sector(first_cluster, private);
    directory->starting_sector_pos = starting_sector_of_directory;
    int total = fat16_get_total_items_for_directory(starting_sector_of_directory, private);
    directory->total = total;
    uint32_t directory_size = total * (sizeof(struct fat_directory_item));
    directory->ending_sector_pos = starting_sector_of_directory + directory_size / private->header.primary_header.BPB_bytes_per_sec;
    struct fat_directory_item *f_item = kzalloc(directory_size); // Made changes here

    if (!f_item) // Made changes here
    {
        res = -ENOMEM;
        goto out;
    }
    directory->fat_item = f_item;
    res = fat16_read_internal(first_cluster, 0x00, directory_size, private, directory->fat_item);

out:
    if (res < ALL_OK)
    {
        kfree_directory(directory); // Added
    }

    return directory;
}

struct fat_directory_item *fat16_clone_file(struct fat_directory_item *item, size_t size)
{
    struct fat_directory_item *clone_item = kzalloc(sizeof(struct fat_directory_item));
    if (!clone_item)
    {
        return 0;
    }
    memncpy(clone_item, item, sizeof(item));
    return clone_item;
}

struct fat_item *fat16_new_fat_item_for_directory_item(struct fat_directory_item *item, struct fat_private *private)
{
    struct fat_item *f_item = 0;
    if (item->attribute & FAT16_FILE_SUBDIRECTORY)
    {
        f_item = kzalloc(sizeof(struct fat_item));

        if (!f_item)
        {
            return 0;
        }

        f_item->directory = fat16_load_directory(item, private);
        f_item->item_type = FAT16_ITEM_TYPE_DIRECTORY;
        return f_item;
    }
    f_item->item = fat16_clone_file(item, sizeof(item));
    f_item->item_type = FAT16_ITEM_TYPE_FILE;
    return f_item;
}

struct fat_item *fat16_find_item_in_directory(struct fat_directory *directory, struct fat_private *private, struct path_part *part)
{
    struct fat_item *fat_item = 0;

    for (int i = 0; i < directory->total; i++)
    {
        struct fat_directory_item *item = &directory->fat_item[i];
        char tmp_name[PATH_NAME_LIMIT];
        fat16_get_full_relative_filename(item, tmp_name, sizeof(tmp_name));
        if (str_n_cmp(tmp_name, part->name, sizeof(part->name) == 0))
        {
            fat_item = fat16_new_fat_item_for_directory_item(item, private);
            break;
        }
    }
    return fat_item;
}

struct fat_item *fat16_get_directory_entry(struct disk *disk, struct path_part *part)
{
    struct fat_private *private = disk->fs_private;
    struct fat_directory *root_directory = &private->root_directory;
    struct fat_item *cur_item = 0;
    struct fat_item *temp_item = 0;
    struct path_part *next_part = part->next;
    struct fat_item *f_item = fat16_find_item_in_directory(root_directory, private, part);
    if (!f_item)
    {
        goto out;
    }
    cur_item = f_item;
    while (next_part != 0)
    {
        if (cur_item->item_type != FAT16_ITEM_TYPE_DIRECTORY)
        {
            cur_item = 0;
            break;
        }
        temp_item = fat16_find_item_in_directory(cur_item->directory, private, next_part);
        fat16_free_fat_item(cur_item); // Added
        cur_item = temp_item;
        next_part = next_part->next;
    }
out:
    return cur_item;
}
void *fat16_open(struct disk *disk, struct root_path *root, FILE_MODE mode)
{
    struct fat16_fs_desc *desc = 0;
    int err_code = 0;
    if (mode != FILE_MODE_READ)
    {
        err_code = -ERDONLY;
        goto err_out;
    }
    desc = kzalloc(sizeof(struct fat16_fs_desc));
    if (!desc)
    {
        err_code = -ENOMEM;
        goto err_out;
    }
    desc->item = fat16_get_directory_entry(disk, root->part); // Need to recheck this.
    if (!desc->item)
    {
        err_code = -EIO;
        goto err_out;
    }
    desc->pos = 0;
    return desc;

err_out:
    if (desc)
    {
        kfree(desc);
    }
    return ERROR(err_code);
}

int fat16_read(struct disk *disk, void *desc, uint32_t size, uint32_t nmemb, char *out_ptr)
{
    struct fat16_fs_desc *fat_desc = desc;
    struct fat_directory_item *item = fat_desc->item->item;
    int res = 0;
    int offset = fat_desc->pos;
    for (int i = 0; i < nmemb; i++)
    {
        res = fat16_read_internal(get_first_cluster_of_directory(item), offset, size, disk->fs_private, out_ptr);
        if (res < 0)
        {
            goto out;
        }
        offset += size;
        out_ptr += size;
    }
    res = nmemb;
out:
    return res;
}

int fat16_seek(void *desc, uint32_t offset, FILE_SEEK_MODE mode)
{
    int res = 0;
    struct fat16_fs_desc *descriptor = desc;
    struct fat_item *f_item = descriptor->item;
    if (f_item->item_type != FAT16_ITEM_TYPE_FILE)
    {
        res = -EINVARG;
        goto out;
    }
    if (offset >= f_item->item->filesize)
    {
        res = -EIO;
        goto out;
    }
    switch (mode)
    {
    case SEEK_SET:
        descriptor->pos = offset;
        break;
    case SEEK_END:
        res = -EUNIMP;
        break;
    case SEEK_CUR:
        descriptor->pos += offset;
        break;
    default:
        res = -EINVARG;
        break;
    }
out:
    return res;
}

int fat16_stat(struct disk *disk, void *desc, struct file_stat *stat)
{
    struct fat16_fs_desc *fat_desc = desc;
    struct fat_directory_item *item = fat_desc->item->item;
    int res = 0;
    if (fat_desc->item->item_type != FAT16_ITEM_TYPE_FILE)
    {
        res = -EINVARG;
        goto out;
    }
    stat->filesize = item->filesize;
    stat->flags = 0x00;
    if (item->attribute & FAT16_FILE_READ_ONLY)
    {
        stat->flags |= FILE_STAT_READ_ONLY;
    }
out:
    return res;
}

int fat16_close(void *desc)
{
    struct fat16_fs_desc *fat_desc = desc;
    int res = 0;
    fat16_free_fat_desc(fat_desc);
    return res;
}
