#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <stddef.h>
#include "../config.h"
#include "../memory/memory.h"
#include "../kernel.h"

struct idt_desc{
    uint16_t offset_first;
    uint16_t segment_selector;
    uint8_t zero;
    uint8_t type_attribute;
    uint16_t offset_second;
} __attribute__((packed));

struct idt_desc_table{
    uint16_t size;
    uint32_t addr;
    

} __attribute__((packed));

void idt_initialization();
void idt_load();

#endif