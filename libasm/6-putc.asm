; 6-putc.asm
; size_t asm_putc(int c);

section .data
    char_buffer db 0   ; Single-byte buffer

section .text
    global asm_putc
    global asm_write_char

asm_write_char:
    ; returning directly to asm_putc's caller.
    mov rax, 1      ; Syscall number for write
    mov rdi, 1      ; stdout file descriptor
    ; rsi is already set up correctly by asm_putc
    mov rdx, 1      ; Number of bytes to write (1)
    syscall         ; Perform the write
    ret             ; Return to C caller (NOT back to asm_putc)

asm_putc:
    push rbp          ;save rbp
    mov rbp, rsp       ;set up stack frame
    sub rsp, 8    ;align stack

    ; Store character in the buffer (RIP-relative addressing).
    mov byte [rel char_buffer], dil

    ; Prepare for the jump to asm_write_char
    lea rsi, [rel char_buffer] ; Load the *address* of char_buffer into rsi

    ; Jump by Van Halen
    jmp asm_write_char

    ; We will NEVER get here unless of course it doesn't work. So its just fun
    add rsp, 8
    mov rsp, rbp
    pop rbp
    ret
