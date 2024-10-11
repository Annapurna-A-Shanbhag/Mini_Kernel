#include "kheap.h"

struct heap_table kernel_table;
struct heap kernel_heap;

void kheap_initialization()
{
    memnset(&kernel_table, 0, sizeof(kernel_table));

    kernel_table.table_entry = (HEAP_BLOCK_TABLE_ENTRY *)HEAP_TABLE_ADDRESS;
    kernel_table.table_size = HEAP_SIZE / HEAP_BLOCK_SIZE;
    size_t total = sizeof(HEAP_BLOCK_TABLE_ENTRY) * kernel_table.table_size;
    void *end_addr = (void *)(HEAP_ADDRESS + HEAP_SIZE);

    int res = heap_initialization(&kernel_heap, &kernel_table, end_addr, total);
    if (res < 0)
    {
        print("Heap Initialization failed\n");
    }
}

void *kmalloc(size_t size)
{

    void *ptr = heap_malloc(&kernel_heap, size);
    return ptr;
}

void *kzalloc(size_t size)
{
    void *ptr = kmalloc(size);
    if (!ptr)
    {
        print("kmalloc has failed\n");
        return 0;
    }
    memnset(ptr, 0, size);
    return ptr;
}

int kfree(void *ptr)
{
    int res = heap_free(&kernel_heap, ptr);
    if (res < 0)
    {
        print("Kfree has failed\n");
    }
    return 0;
}