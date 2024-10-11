#include "kernel.h"

uint16_t *video_mem = 0;
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
    video_mem = (uint16_t *)(0xB8000);
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

void print(char *str)
{
    size_t len = str_len(str);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}

void print_c(char *str1,int c){
    int i=0;
    char str2[PATH_LIMIT];
    while(str1[i]!='\0'){
        str2[i]=str1[i];
        i++;
    }
    str2[i]=c+65;
    str2[i+1]='\0';
    print(str2);
}

void panic(char *str){
    print(str);
    while(1){}
}

struct tss tss;
struct gdt gdt[GDT_TOTAL_ENTRIES];
struct gdt_structure gdt_structure[GDT_TOTAL_ENTRIES] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                 // NULL
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},           // Kernel_CODE
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},           // Kernel_DATA
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},           // User_CODE
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},           // User_DATA
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xe9} // Tss

};

static struct paging_4gb_chunk *kernel_chunk = 0;

void kernel_page()
{
    kernel_registers();
    paging_switch(kernel_chunk);
}
void kernel_main()
{

    terminal_initialize();
    //print("HELLO WORLD\n");
    
    memnset(gdt, 0, sizeof(gdt));
    
    gdt_initialize(gdt, gdt_structure);
   
    
    gdt_load(sizeof(gdt), gdt);
    

    kheap_initialization();

    fs_initialization();
    //fs_init();
    disk_search_and_init();

    
    idt_initialization();

    memnset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000;
    tss.ss0 = KERNEL_DATA_SEGMENT_SELECTOR;

    //Load the TSS
    tss_load(0x28);

    kernel_chunk = paging_new_4gb(PAGING_IS_PRESENT | PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL);

    paging_switch(kernel_chunk);
    enable_paging(); 

    isr80h_register_commands();

    keyboard_initialization();
    

   struct process *process;
    int res=process_load_switch("0:/shell.elf",&process);
    if(res<0){
        panic("Process isn't loaded\n");
    }
    struct command_arguments argument;
    str_n_cpy(argument.argument,"ABC!",4);
    argument.next=0;
    process_inject_arguments(process,&argument); 

    

    /*res=process_load_switch("0:/blank.elf",&process);
    if(res<0){
        panic("Process isn't loaded\n");
    }
    str_n_cpy(argument.argument,"XYZ!",4);
    argument.next=0;
    process_inject_arguments(process,&argument); */

    task_run_first_ever_task();
    while(1){} 


}