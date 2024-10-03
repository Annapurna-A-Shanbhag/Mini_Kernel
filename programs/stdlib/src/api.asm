[BITS 32]

section .asm

; void print(char *str)
print:
    push ebp
    mov ebp,esp
    push dword[ebp+8] 
    mov eax,1
    int 0x80
    add esp,4
    pop ebp
    ret

; int get_key()
getkey:
    push ebp
    mov ebp,esp
    mov eax,2
    int 0x80
    pop ebp
    ret

;  void putchar(char c)
putchar:
    push ebp
    mov ebp,esp
    push dword[ebp+8]
    mov eax,3
    int 0x80
    add esp,4
    pop ebp
    ret

; void malloc(size_t size)
malloc:
    push ebp
    mov ebp,esp
    push dword[ebp+8]
    mov eax,4
    int 0x80
    add esp,4
    pop ebp
    ret

; void free(void * addr)
free:
    push ebp
    mov ebp,esp
    push dword[ebp+8]
    mov eax,5
    int 0x80
    add esp,4
    pop ebp
    ret

; void process_load_start(char *filename)
process_load_start:
    push ebp
    mov ebp,esp
    push dword[ebp+8]
    mov eax,6
    int 0x80
    add esp,4
    pop ebp
    ret

; int invoke_system_command(struct command_argument *argument)
invoke_system_command:
    push ebp
    mov ebp,esp
    push dword[ebp+8]
    mov eax,7
    int 0x80
    add esp,4
    pop ebp
    ret

; void process_get_arguments(struct process_arguments* arguments)
process_get_arguments:
    push ebp
    mov ebp, esp
    mov eax, 8 
    push dword[ebp+8] 
    int 0x80
    add esp, 4
    pop ebp
    ret

; void exit()
exit:
    push ebp
    mov ebp, esp
    mov eax, 9 
    int 0x80
    pop ebp
    ret

