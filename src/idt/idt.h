#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <stddef.h>
#include "../config.h"
#include "../memory/memory.h"
#include "../kernel.h"
#include "../task/task.h"

#include "../task/process.h"
#include "../io/io.h"

struct interrupt_frame;
typedef void *(*ISR80H_COMMAND)(struct interrupt_frame *frame);
typedef void (*INTERRUPT_CALLBACK_FUNCTION)();

struct idt_desc
{
    uint16_t offset_first;
    uint16_t segment_selector;
    uint8_t zero;
    uint8_t type_attribute;
    uint16_t offset_second;
} __attribute__((packed));

struct idt_desc_table
{
    uint16_t size;
    uint32_t addr;

} __attribute__((packed));

struct interrupt_frame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

void idt_initialization();
void idt_load(struct idt_desc_table *ptr);
int idt_register_interrupt_callback(int interrupt_no, INTERRUPT_CALLBACK_FUNCTION ISR);
void register_isr80h_command(int command_no, ISR80H_COMMAND command);

#endif