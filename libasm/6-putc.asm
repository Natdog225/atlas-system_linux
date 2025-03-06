; 6-putc.asm

section .data
    char_buffer db 0   ; Single-byte buffer

section .text
    global asm_putc
    global asm_write_char

asm_write_char:
    push rbp         ; Save rbp
    mov rbp, rsp     ; set up stack frame

    mov rax, 1          ; Syscall number for write
    mov rdi, 1          ; stdout file descriptor
    mov rsi, [rbp + 16] ; Third argument is the buffer address
    mov rdx, 1          ; Number of bytes to write (1)
    syscall             ; Perform the write

    mov rsp, rbp     ; Restore the stack
    pop rbp           ; Restore rbp

    ret          ; Return

asm_putc:
    push rbp           ; Save the caller's base pointer
    mov  rbp, rsp      ; Set up our stack frame
    sub  rsp, 8          ;create space for local and align the stack

    mov byte [rel char_buffer], dil  ; Store the character

    lea rdi, [rel char_buffer]   ; Get address of the buffer
    push rdi                 ;put buffer address to stack
    call asm_write_char         ;call to function
    add rsp, 8        ;clean stack

    mov rsp, rbp      ; Restore stack pointer
    pop rbp           ; Restore base pointer
    ret               ; Return to the C caller