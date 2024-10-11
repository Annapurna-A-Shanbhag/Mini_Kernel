[BITS 32]

global _start

extern c_main
extern exit

section .asm

_start:
    call c_main
    call exit
    ret
