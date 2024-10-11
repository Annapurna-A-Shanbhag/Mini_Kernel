#include "gdt.h"


void gdt_encode(uint8_t * target,struct gdt_structure *source ){


    if ((source->limit > 65536) && ((source->limit & 0xFFF) != 0xFFF))
    {
        panic("encodeGdtEntry: Invalid argument\n");
    }
    target[0]=source->limit & 0xff;         //limit_first_byte
    target[1]=(source->limit>>8)& 0xff;     //limit_second_byte
    target[2]=source->base & 0xff;          //base_first_byte
    target[3]=(source->base>>8)& 0xff;      //base_second_byte
    target[4]=(source->base>>16)& 0xff;     //base_third_byte
    target[5]=source->type;                  //access_code;
    target[6] = 0x40; 

    if (source->limit > 65536){
        source->limit = source->limit >> 12;
        target[6] = 0xC0;
    }
    
    target[6] |= (source->limit >> 16) & 0x0F; //flag
    target[7]=(source->base>>24)& 0xff;       //base_last_byte

}


void gdt_initialize(struct gdt *gdt, struct gdt_structure *gdt_structure){
    for(int i=0; i< GDT_TOTAL_ENTRIES;i++){
        gdt_encode((uint8_t *)&gdt[i],&gdt_structure[i]);
    }
}