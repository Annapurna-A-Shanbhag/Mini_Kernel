#include "elfloader.h"

const char elf_signature[] = {0x7f, 'E', 'L', 'F'};

void *elf_memory(struct elf_file *elf_file)
{
    return elf_file->elf_memory;
}

void *elf_header(struct elf_file *elf_file)
{
    return elf_file->elf_memory;
}

int elf_validate_signature(void *buffer)
{

    return memcmp(buffer, (void *)elf_signature, sizeof(elf_signature)) == 0;
}

int elf_validate_class(struct elf_header *header)
{
    return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

int elf_validate_encoding(struct elf_header *header)
{
    return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}
struct elf32_phdr *elf_pheader(struct elf_header *header)
{
    if (header->e_phoff == 0)
    {
        return 0;
    }

    return (struct elf32_phdr *)((int)header + header->e_phoff);
}
int elf_validate_loaded(struct elf_header *header)
{
    if (elf_validate_signature(header) && elf_validate_class(header) && elf_validate_encoding(header))
    {
        return ALL_OK;
    }
    else
        return -EINFORMAT;
}

struct elf32_phdr *elf_program_header(struct elf_header *header, int index)
{

    return &elf_pheader(header)[index];
}

int elf_process_phdr_pt_load(struct elf_file *elf_file, struct elf32_phdr *phdr)
{
    if (elf_file->virtual_start_address >= (void *)phdr->p_vaddr || elf_file->virtual_start_address == 0x00)
    {
        elf_file->virtual_start_address = (void *)phdr->p_vaddr;
        elf_file->physical_start_address = elf_memory(elf_file) + phdr->p_offset;
    }

    unsigned int end_virtual_address = phdr->p_vaddr + phdr->p_filesz;
    if (elf_file->virtual_end_address <= (void *)(end_virtual_address) || elf_file->virtual_end_address == 0x00)
    {
        elf_file->virtual_end_address = (void *)end_virtual_address;
        elf_file->physical_end_address = elf_memory(elf_file) + phdr->p_offset + phdr->p_filesz;
    }
    return 0;
}

int elf_process_pheader(struct elf_file *elf_file, struct elf32_phdr *phdr)
{
    int res = 0;
    switch (phdr->p_type)
    {
    case PT_LOAD:
        res = elf_process_phdr_pt_load(elf_file, phdr);
        break;
    }
    return res;
}

int elf_process_pheaders(struct elf_file *elf_file)
{
    int res = 0;
    struct elf_header *header = elf_header(elf_file);
    for (int i = 0; i < header->e_phnum; i++)
    {
        struct elf32_phdr *phdr = elf_program_header(header, i);
        res = elf_process_pheader(elf_file, phdr);
        if (res < 0)
        {
            break;
        }
    }

out:
    return res;
}

int elf_process_loaded(struct elf_file *elf_file)
{

    int res = 0;
    struct elf_header *header = elf_memory(elf_file);
    res = elf_validate_loaded(header);
    if (res < 0)
    {
        goto out;
    }

    res = elf_process_pheaders(elf_file);
out:
    return res;
}

int elf_load(char *filename, struct elf_file **elf)
{
    struct elf_file *elf_file = kzalloc(sizeof(struct elf_file));
    int res = 0;
    int fd = fopen(filename, "r");
    if (fd <= 0)
    {
        goto out;
    }
    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res < 0)
    {
        goto out;
    }
    elf_file->elf_memory = kzalloc(sizeof(stat.filesize));
    res = fread(elf_file->elf_memory, stat.filesize, 1, fd);
    if (res < 0)
    {
        goto out;
    }
    res = elf_process_loaded(elf_file);
    if (res < 0)
    {
        goto out;
    }
    *elf = elf_file;

out:
    fclose(fd);
    return res;
}

void elf_close(struct elf_file *file)
{
    if (!file)
        return;

    kfree(file->elf_memory);
    kfree(file);
}