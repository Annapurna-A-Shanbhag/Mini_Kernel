[BITS 32]

section .asm
global first



first:
    mov eax,message
    mov dword [eax],'ANNA'
    mov dword [eax+4],'PURN'
    mov dword [eax+8],'A'
    push eax
    mov eax,1
    int 0x080
    add esp,4
    

section .data
message db 'ANNAPURNA',0

