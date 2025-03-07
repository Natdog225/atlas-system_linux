; 6-putc.asm
; size_t asm_putc(int c);

section .data
    char_buffer db 0   ; Single-byte buffer

section .text
    global asm_putc

asm_putc:
    push rbp           ; Save caller's base pointer.
    mov rbp, rsp       ; set up stack frame
    sub rsp, 8    ; Align stack to 16 bytes

    mov byte [rel char_buffer], dil  ; (RIP-relative)

    ; Prepare for the jump to asm_write_char
    lea rsi, [rel char_buffer]   ; Get the address of the buffer

    ; Prepare arguments for the syscall
    mov rax, 1          ; Syscall number for write
    mov rdi, 1          ; stdout file descriptor
    ; rsi holds the buffer address
    mov rdx, 1          ; Number of bytes to write (1)

    ; Perform the system call directly. NO JMP by van halen
    syscall

    ; --- Cleanup and Return ---
    add rsp, 8             ; Restore stack
    mov rsp, rbp           ; restore stack pointer
    pop rbp            ; Restore base pointer.
    ret               ; Return to C caller.