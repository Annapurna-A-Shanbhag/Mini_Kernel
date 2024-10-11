section .asm

global idt_load
global interrupt_pointer_table
global isr80h_wrapper
global no_interrupt
global isr

extern interrupt_handler
extern isr80h_handler
extern no_interrupt_handler

idt_load:
    push ebp
    mov ebp,esp
    mov ebx,[ebp+8];
    lidt [ebx]
    pop ebp
    ret



no_interrupt:
    pushad
    call no_interrupt_handler
    popad
    iret

isr:
    call no_interrupt_handler
    iret

isr80h_wrapper:
     ; INTERRUPT FRAME START
     ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
     ; uint32_t ip
     ; uint32_t cs;
     ; uint32_t flags
     ; uint32_t sp;
     ; uint32_t ss;
     ; Pushes the general purpose registers to the stack
     pushad
     push esp
     push eax
     call isr80h_handler
     mov dword[tmp_res], eax
     add esp,8
     popad
     mov eax,[tmp_res]
     iretd


%macro interrupt 1
    global int%1
    int%1: 
        ; INTERRUPT FRAME START
        ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
        ; uint32_t ip
        ; uint32_t cs;
        ; uint32_t flags
        ; uint32_t sp;
        ; uint32_t ss;
        ; Pushes the general purpose registers to the stack
        pushad
        ; Interrupt frame end
        push esp
        push dword %1
        call interrupt_handler
        add esp,8
        popad
        iret
%endmacro

%assign i 0
%rep 512
    interrupt i
%assign i i+1
%endrep



     
section .data
; Inside here is stored the return result from isr80h_handler
tmp_res: dd 0

%macro interrupt_array_entry 1
    dd int%1
%endmacro


interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep

