#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include "../config.h"
#include "../kernel.h"

struct gdt{
    uint16_t limit_first_word;
    uint16_t base_first_word;
    uint8_t  base_next_byte;
    uint8_t  access_code;
    uint8_t  flags;
    uint8_t  base_last_byte;

};

struct gdt_structure{
    uint32_t base;
    uint32_t limit;
    uint8_t  type;

};

//void gdt_encode(uint8_t * target,struct gdt_structure source );
void gdt_initialize(struct gdt *gdt, struct gdt_structure *gdt_structure);
void gdt_load(uint32_t size,struct gdt *gdt);


#endif
