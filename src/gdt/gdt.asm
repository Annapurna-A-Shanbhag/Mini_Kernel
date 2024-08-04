section .asm

global gdt_load

gdt_load:
    mov ax,[esp+4]
    mov [gdt_descriptor],ax
    mov eax,[esp+8]
    mov [gdt_descriptor+2],eax
    lgdt [gdt_descriptor]
    ret

section .data

gdt_descriptor:
    dw 0x0
    dd 0x0