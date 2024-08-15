#include "kernel.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour)
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}
void terminal_writechar(char c, char colour)
{
    if (c == '\n')
    {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }


    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1;
     }
}
void terminal_initialize()
{
    video_mem = (uint16_t*)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }   
}

void print(char* str)
{
   size_t len = str_len(str);
   for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    } 
    
}


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


    terminal_initialize();
    print("HELLO WORLD\n");
    memnset(gdt,0,sizeof(gdt));
    gdt_initialize(gdt,gdt_structure);
    gdt_load(sizeof(gdt),gdt);

    idt_initialization();
    kheap_initialization();
    struct paging_4gb_chunk *chunk=paging_new_4gb(PAGING_IS_PREENT | PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL);
    paging_switch(chunk);
    //enable_paging();

    fs_initialization();
    disk_search_and_init();

    

    
    

}