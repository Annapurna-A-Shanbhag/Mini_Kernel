#include "process.h"

struct process *processes[NUMBER_OF_TOTAL_PROCESSES] = {};
struct process *current_process = 0;

void process_get_arguments(struct process *process, int *argc, char ***argv)
{

    argc = process->arguments.argc;
    argv = process->arguments.argv;
}

int process_get_number_of_arguments(struct command_arguments *root_argument)
{
    struct command_arguments *current = root_argument;
    int count = 0;
    while (current)
    {
        current = current->next;
        count++;
    }
    return count;
}

int process_inject_arguments(struct process *process, struct command_arguments *root_argument)
{
    int res = 0;
    int argc = process_get_number_of_arguments(root_argument);
    if (argc == 0)
    {
        res = -EINVARG;
        goto out;
    }
    char **argv = process_malloc(process, argc * sizeof(char *));
    struct command_arguments *current = root_argument;
    int i = 0;
    while (current)
    {
        char *arg = process_malloc(process, sizeof(current->argument));
        str_n_cpy(arg, current->argument, sizeof(current->argument));
        argv[i] = arg;
        current = current->next;
        i++;
    }
    process->arguments.argc = argc;
    process->arguments.argv = argv;
out:
    return res;
}

struct process *process_get_process_by_id(uint16_t process_slot)
{
    if (process_slot < 0 || process_slot >= NUMBER_OF_TOTAL_PROCESSES)
    {
        return -EINVARG;
    }

    return processes[process_slot];
}

struct process_allocation *process_get_alloc_index(struct process *process)
{
    for (int i = 0; i < NUMBER_OF_PROCESS_ALLOCATION; i++)
    {
        if (process->allocations[i].addr == 0)
        {
            return &process->allocations[i];
        }
    }
    return 0;
}

int process_malloc(struct process *process, size_t size)
{
    int res = 0;
    struct process_allocation *allocation = process_get_free_alloc_index(process);
    if (allocation == 0)
    {
        res = -ENOMEM;
        goto out;
    }
    void *phy = kzalloc(sizeof(size));
    if (!phy)
    {
        res = -ENOMEM;
        goto out;
    }
    res = paging_map_to(process->task->chunk, phy, phy, paging_align_address((uint32_t)phy + size), PAGING_IS_PREENT | PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL);
    if (res < 0)
    {
        res = -EIO;
        goto out;
    }
    allocation->addr = phy;
    allocation->size = size;

out:
    if (res < 0)
    {
        if (phy)
            kfree(phy);
    }
    return res;
}

struct process_allocation *process_get_alloc_by_addr(struct process *process, void *ptr)
{
    for (int i = 0; i < NUMBER_OF_PROCESS_ALLOCATION; i++)
    {
        if (process->allocations[i].addr == ptr)
        {
            return &process->allocations[i];
        }
    }
    return -EIO;
}

void process_allocation_unjoin(struct process_allocation *allocation)
{
    allocation->addr = 0;
    allocation->size = 0;
}

int process_free(struct process *process, void *ptr)
{
    int res = 0;
    struct process_allocation *allocation = process_get_alloc_by_addr(process, ptr);
    if (!allocation)
    {
        res = -EIO;
        goto out;
    }
    res = paging_map_to(process->task->chunk, ptr, ptr, paging_align_address((uint32_t)allocation->addr + allocation->size), 000);
    if (res < 0)
    {
        res - EIO;
        goto out;
    }
    process_allocation_unjoin(allocation);
    kfree(ptr);

out:
    return res;
}

void process_switch(struct process *process)
{
    current_process = process;
}

void process_switch_to_any()
{
    for (int i = 0; i < NUMBER_OF_TOTAL_PROCESSES; i++)
    {
        if (processes[i])
        {
            process_switch(processes[i]);
        }
    }
}

void process_unlink(struct process *process)
{
    processes[process->pid] = 0x00;
    if (current_process == process)
    {
        process_switch_to_any();
    }
}

int process_free_allocations(struct process *process)
{
    for (int i = 0; i < NUMBER_OF_PROCESS_ALLOCATION; i++)
    {
        process_free(process, process->allocations[i].addr);
    }
    return 0;
}

void process_free_binary_data(struct process *process)
{
    kfree(process->phy_addr);
}

int process_free_elf_data(struct process *process)
{
    elf_close(process->elf_file);
    return 0;
}

void process_free_program_data(struct process *process)
{
    int res = 0;
    switch (process->filetype)
    {
    case PROCESS_BINARY_FILE:
        res = process_free_binary_data(process);
        break;

    case PROCESS_ELF_FILE:
        res = process_free_elf_data(process);
        break;

    default:
        res = -EINVARG;
    }
    return res;
}

int process_terminate(struct process *process)
{
    int res = 0;
    res = process_free_allocations(process);
    process_free_program_data(process);
    kfree(process->stack_phy_address);
    task_free(process->task);
    process_unlink(process);
}

int process_map_binary(struct process *process)
{
    return paging_map_to(process->task->chunk, process->phy_addr, (void *)PROGRAM_VIRTUAL_ADDRESS, paging_align_address(process->phy_addr + process->size), PAGING_IS_PREENT | PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL);
}

static int process_map_elf(struct process *process)
{
    int res = 0;

    struct elf_file *elf_file = process->elf_file;
    struct elf_header *header = elf_header(elf_file);
    struct elf32_phdr *phdrs = elf_pheader(header);
    for (int i = 0; i < header->e_phnum; i++)
    {
        struct elf32_phdr *phdr = &phdrs[i];
        void *phdr_phys_address = elf_phdr_phys_address(elf_file, phdr);
        int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
        if (phdr->p_flags & PF_W)
        {
            flags |= PAGING_IS_WRITABLE;
        }
        res = paging_map_to(process->task->chunk, paging_align_to_lower_page((void *)phdr->p_vaddr), paging_align_to_lower_page(phdr_phys_address), paging_align_address(phdr_phys_address + phdr->p_memsz), flags);
        if (ISERR(res))
        {
            break;
        }
    }
    return res;
}

int process_map_memory(struct process *process)
{
    int res = 0;
    if (process->filetype == PROCESS_ELF_FILE)
    {
        res = process_map_elf();
    }
    else if (process->filetype == PROCESS_BINARY_FILE)
    {
        process_map_binary(process);
    }
    else
    {
    }
    if (res < 0)
    {
        goto out;
    }

    // Finally map the stack
    paging_map_to(process->task->chunk, (void *)PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack_phy_address, paging_align_address(process->stack_phy_address + USER_PROGRAM_STACK_SIZE), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);
out:
    return res;
}

int process_load_binary(char *filename, struct process *process)
{
    void *program_data_ptr = 0;
    int res = 0;
    int fd = fopen(filename, 'r');
    if (!fd)
    {
        res = -EIO;
        goto out;
    }
    struct file_stat stat; // Don't use pointer here
    res = fstat(fd, &stat);
    if (!res)
    {
        res = -EIO;
        goto out;
    }
    program_data_ptr = kzalloc(sizeof(stat.filesize));
    res = fread(program_data_ptr, stat.filesize, 1, fd);
    if (!res)
    {
        res = -EIO;
        goto out;
    }
    process->phy_addr = program_data_ptr;
    process->size = stat.filesize;
    process->filetype = PROCESS_BINARY_FILE;

out:
    if (res < 0)
    {
        if (program_data_ptr)
            kfree(program_data_ptr);
    }
    fclose(fd);
    return res;
}

int process_load_elf_data(char *filename, struct process *process)
{
    struct elf_file *elf_file = 0;
    int res = 0;
    res = elf_load(filename, &elf_file);
    if (res < 0)
    {
        goto out;
    }
    process->filetype = PROCESS_ELF_FILE;
    process->elf_file = elf_file;

out:
    return res;
}

int process_load_data(char *filename, struct process *process)
{
    int res = 0;
    res = process_load_binary_data(filename, process);
    if (res < 0)
    {
        res = process_load_elf_data(filename, process);
    }
    return res;
}

int process_load_for_slot(char *filename, struct process **process, uint16_t process_slot)
{
    struct process *_process;
    int res = 0;
    res = process_get_process_by_id(process_slot);
    if (res <= 0)
    {
        goto out;
    }

    _process = kzalloc(sizeof(struct process));
    if (!_process)
    {
        res = -ENOMEM;
        goto out;
    }

    void *program_physical_stack = kzalloc(sizeof(USER_PROGRAM_STACK_SIZE));
    if (!program_physical_stack)
    {
        res = -ENOMEM;
        goto out;
    }

    res = process_load_data(filename, _process);
    if (res < 0)
    {
        goto out;
    }

    struct task *task = task_new(_process);
    _process->task = task;
    _process->stack_phy_address = program_physical_stack;
    str_n_cpy(_process->filename, filename, sizeof(filename));
    _process->pid = process_slot;

    res = process_map_memory(_process);
    if (res < 0)
    {
        goto out;
    }
    *process = _process;
    processes[process_slot] = process; // What about current process??

out:
    return res;
}

int process_get_free_slot()
{
    for (int i = 0; i < NUMBER_OF_TOTAL_PROCESSES; i++)
    {
        if (processes[i] == 0x0)
        {
            return i;
        }
    }
    return -ENOMEM;
}

int process_load(char *filename, struct process **process)
{
    int res = 0;
    int p_slot = process_get_free_slot();
    if (p_slot < 0)
    {
        res = -EISTKN;
        goto out;
    }
    res = process_load_for_slot(filename, process, p_slot);

out:
    return res;
}

int process_load_switch(char *filename, struct process **process)
{
    int res = process_load(filename, process);
    if (res == 0)
    {
        process_switch(*process);
    }

    return res;
}
