#include "kernel.h"


struct tss tss;
struct gdt gdt[GDT_TOTAL_ENTRIES];
struct gdt_structure gdt_structure[GDT_TOTAL_ENTRIES]={
    {.base=0x00,.limit=0x00,.type=0x00},                   //NULL
    {.base=0x00,.limit=0xffffffff,.type=0x9a},             //Kernel_CODE
    {.base=0x00,.limit=0xffffffff,.type=0x92},             //Kernel_DATA
    {.base=0x00,.limit=0xffffffff,.type=0xf8},             //User_CODE
    {.base=0x00,.limit=0xffffffff,.type=0xf2},             //User_DATA
    {.base=(uint32_t)&tss,.limit=sizeof(tss),.type=0xe9}   //Tss

};
void kernel_main(){

    gdt_initialize(gdt,gdt_structure);
    gdt_load(sizeof(gdt),gdt);

}