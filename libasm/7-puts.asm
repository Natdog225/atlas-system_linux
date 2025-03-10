section .text
global asm_puts
extern asm_strlen

asm_puts:
    push rbp
    mov rbp, rsp
    
    ; Save registers
    push rbx
    push r12
    push r13

    ; Get string length
    mov r12, rdi        ; Save original string pointer
    call asm_strlen     ; Returns length in rax
    mov r13, rax        ; Save string length

    ; Setup syscall
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, r12        ; string pointer
    mov rdx, r13        ; exact string length (no newline)

    ; Make the system call
    syscall

    ; Return the string length (not length + 1)
    mov rax, r13        ; Return original string length

cleanup:
    ; Restore registers
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
