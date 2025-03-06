; 6-putc.asm
; size_t asm_putc(int c);

section .data
    char_buffer db 0   ; Single-byte buffer

section .text
    global asm_putc

asm_putc:
    push rbp
    mov rbp, rsp

    ; Load the character into the buffer (using a register)
    mov byte [rel char_buffer wrt ..got], dil

    ; Prepare the system call (write)
    mov eax, 1          ; Syscall number for write
    mov edi, 1          ; stdout file descriptor
    lea rsi, [rel char_buffer wrt ..got]  ; Load the *effective* address of char_buffer
    mov edx, 1          ; Number of bytes to write

    ; Perform the system call
    syscall

    ; rax already contains the return value (bytes written)
    mov rsp, rbp
    pop rbp
    ret