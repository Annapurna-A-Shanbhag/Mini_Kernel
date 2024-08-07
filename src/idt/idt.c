#include "idt.h"


struct idt_desc idt[TOTAL_NO_INTERRUPTS];
struct idt_desc_table idtr;
void initial(){
    print("IDT Initialization\n");
}

void idt_set(int index){
    idt[index].offset_first=((uint32_t)initial)& 0x0000ffff;
    idt[index].segment_selector=KERNEL_CODE_SEGMENT_SELECTOR;
    idt[index].zero=0x0;
    idt[index].type_attribute=0xEE;
    idt[index].offset_second=((uint32_t)initial)>>16;
    
}

void idt_initialization(){
    memnset(idt,0,sizeof(idt));
    
    for(int i=0;i<TOTAL_NO_INTERRUPTS;i++){
        idt_set(i);

    }
    idtr.addr=(uint32_t)idt;
    idtr.size=(uint16_t)sizeof(idt);
    
    idt_load(idtr);
    
    
}