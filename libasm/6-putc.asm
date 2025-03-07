; 6-putc.asm
; size_t asm_putc(int c);

section .data
    char_buffer db 0   ; Single-byte buffer

section .text
    global asm_putc
    global asm_write_char

asm_write_char:
    ; returning directly to asm_putc's caller.
    push rbp              ; Preserve rbp
    mov rbp, rsp          ; Set up stack Frame
    mov rax, 1          ; Syscall number for write
    mov rdi, 1          ; stdout file descriptor
    mov rsi, [rbp + 16] ; Load buffer address from the stack -- CORRECTED
    mov rdx, 1          ; Number of bytes to write (1)
    syscall             ; Perform the write
    mov rsp, rbp       ; Restore Stack
    pop rbp            ; Restore rbp
    ret                 ; Return (to the C caller!)

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
