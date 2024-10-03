[BITS 32]

section .asm

global _start

extern c_main
extern exit

_start:
    call c_main
    call exit
    ret
