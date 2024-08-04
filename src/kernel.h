#ifndef KERNEL_H
#define KERNEL_H
#include "../config.h"
#include "stdint.h"
#include "gdt/gdt.h"
#include "./task/tss.h"

void kernel_main();

#endif