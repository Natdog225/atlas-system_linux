section .text
global asm_puts
extern asm_strlen

asm_puts:
    push rbp
    mov rbp, rsp
    
    ; Save only necessary registers
    push rbx
    push r12
    push r13

    ; Get string length
    mov r12, rdi        ; Save string pointer
    call asm_strlen
    mov r13, rax        ; Save string length

    ; Add 1 for newline
    inc r13             ; Total length including newline

    ; Setup syscall
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, r12        ; string pointer
    mov rdx, r13        ; length

    ; Make the system call
    syscall

    ; Restore registers
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
