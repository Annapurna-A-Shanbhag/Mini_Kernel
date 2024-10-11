#include "heap.h"

int heap_validate_address_alignment(void *addr)
{
    return (uint32_t)addr % (uint32_t)HEAP_BLOCK_SIZE;
}

void heap_table_initialization(struct heap_table *table, size_t total)
{
    memnset(table->table_entry, HEAP_BLOCK_TABLE_ENTRY_FREE, total);
}

int heap_initialization(struct heap *heap, struct heap_table *table, void *end_addr, size_t total)
{
    int res = 0;
    memnset(heap, 0, sizeof(heap));
    heap->address = (void *)HEAP_ADDRESS;
    heap->table = table;
    if (heap_validate_address_alignment(heap->address) || heap_validate_address_alignment(end_addr))
    {
        res = -EINVARG;
        goto out;
    }

    heap_table_initialization(table, total);
out:
    return res;
}

int heap_convert_bytes_to_blocks(size_t size)
{
    int blocks = size / HEAP_BLOCK_SIZE;
    int remainder = size % HEAP_BLOCK_SIZE;
    if (remainder != 0)
    {
        blocks++;
    }
    return blocks;
}

int heap_find_first_free_block(struct heap *heap, int blocks)
{
    struct heap_table *table = heap->table;
    int no_of_blocks = 0;
    int index = 0;
    for (int i = 0; i < table->table_size; i++)
    {
        if (no_of_blocks == 0)
        {
            index = i;
        }
        if (table->table_entry[i] == HEAP_BLOCK_TABLE_ENTRY_FREE)
        {
            no_of_blocks++;
            if (blocks == no_of_blocks)
            {
                return index;
            }
        }
        else
        {
            no_of_blocks = 0;
        }
    }
    return -ENOMEM;
}

void *heap_convert_block_to_address(struct heap *heap, int block)
{
    return heap->address + block * HEAP_BLOCK_SIZE;
}

void heap_mark_table_entries_as_taken(struct heap *heap, int first_block, int size)
{
    struct heap_table *table = heap->table;
    int end_block = first_block + size;
    HEAP_BLOCK_TABLE_ENTRY *addr = table->table_entry;
    addr[first_block] = HEAP_BLOCK_IS_FIRST;

    for (int i = first_block; i < end_block; i++)
    {
        addr[i] |= HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (i + 1 != end_block)
        {
            addr[i] |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

void *heap_malloc(struct heap *heap, size_t size)
{
    void *addr = 0;
    int blocks = heap_convert_bytes_to_blocks(size);
    int first_block = heap_find_first_free_block(heap, blocks);
    if (first_block < 0)
    {
        goto out;
    }

    addr = heap_convert_block_to_address(heap, first_block);
    heap_mark_table_entries_as_taken(heap, first_block, blocks);
out:
    return addr;
}

int heap_convert_address_to_blocks(struct heap *heap, void *ptr)
{
    int blocks = (int)(ptr - heap->address) / HEAP_BLOCK_SIZE;
    return blocks;
}

int heap_mark_table_entries_as_free(struct heap *heap, int first_block)
{
    int res = 0;
    HEAP_BLOCK_TABLE_ENTRY *addr = heap->table->table_entry;
    if ((addr[first_block] & HEAP_BLOCK_IS_FIRST) != HEAP_BLOCK_IS_FIRST)
    {
        res = -EINVARG;
        goto out;
    }

    int i = 0;
    int index=i+first_block;
     while ((addr[index] & HEAP_BLOCK_HAS_NEXT) != 0x0)
    {
        addr[index] = HEAP_BLOCK_TABLE_ENTRY_FREE;
        i++;
        index++;
    }
    addr[index] = HEAP_BLOCK_TABLE_ENTRY_FREE;
    res = i+1;

out:
    return res;
}

size_t heap_convert_blocks_to_bytes(int blocks)
{
    return blocks * HEAP_BLOCK_SIZE;
}

void heap_mark_heap_entries_as_free(struct heap *heap, size_t size, void *ptr)
{
    memnset(ptr, 0, size);
}

int heap_free(struct heap *heap, void *ptr)
{
    int res = heap_validate_address_alignment(ptr);
    if (res != 0)
    {
        res = -EINVARG;
        goto out;
    }

    int start_block = heap_convert_address_to_blocks(heap, ptr);
    int blocks = heap_mark_table_entries_as_free(heap, start_block);
    if (blocks < 0)
    {
        res = -EINVARG;
        goto out;
    }

    size_t bytes = heap_convert_blocks_to_bytes(blocks);
    heap_mark_heap_entries_as_free(heap, bytes, ptr);
    res = 0;

out:
    return res;
}