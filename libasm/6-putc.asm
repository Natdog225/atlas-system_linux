; 6-putc.asm
; size_t asm_putc(int c);

section .data
    char_buffer db 0   ; Single-byte buffer

section .text
    global asm_putc
    global asm_write_char  ; Make the write function globally accessible

; This function performs the actual system call.
asm_write_char:
    mov rax, 1          ; Syscall number for write
    mov rdi, 1          ; stdout file descriptor
    mov rsi, rdi        ; Third argument (buffer) passed by the caller
    mov rdx, 1          ; Number of bytes to write (1)
    syscall             ; Perform the write
    ret                 ;return the value of rax.

asm_putc:
    push rbp
    mov rbp, rsp

    ; Load the character into the buffer (using RIP-relative addressing)
    mov byte [rel char_buffer], dil

    ; Prepare arguments for asm_write_char
    ; We'll be jumping, not calling, so we set up the registers directly
    lea rdi, [rel char_buffer] ;  Pass the buffer address to our helper function

    ; Jump to the write function
    jmp asm_write_char        ; Jump to asm_write_char.

    ; The code execution will continue at asm_write_char.
    ; The return from asm_write_char will come back *here*.

    ; rax already contains the return value (bytes written)
    mov rsp, rbp
    pop rbp
    ret
