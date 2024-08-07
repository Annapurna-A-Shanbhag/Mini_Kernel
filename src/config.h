#ifndef CONFIG_H
#define CONFIG_H

#define GDT_TOTAL_ENTRIES 6

#define KERNEL_CODE_SEGMENT_SELECTOR 0x80
#define KERNEL_DATA_SEGMENT_SELECTOR 0x80
#define USER_CODE_SEGMENT_SELECTOR 0x16;
#define USER_DATA_SEGMENT_SELECTOR 0x1E;


#define HEAP_SIZE 104857600
#define HEAP_BLOCK_SIZE 4096
#define HEAP_TABLE_ADDRESS 0x00007e00
#define HEAP_ADDRESS 0x01000000

#define TOTAL_NO_INTERRUPTS 512

#endif