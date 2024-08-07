#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>
#include <stdint.h>
#include "./config.h"
#include "stdint.h"
#include "gdt/gdt.h"
#include "./task/tss.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "string/string.h"
#include "memory/paging/paging.h"
#include "idt/idt.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

void kernel_main();
void print(const char* str);

#endif