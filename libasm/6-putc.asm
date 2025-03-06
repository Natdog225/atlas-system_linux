; 6-putc.asm
; size_t asm_putc(int c);

section .data
    char_buffer db 0   ; Single-byte buffer to hold the character

section .text
    global asm_putc

asm_putc:
    ; --- Setup ---
    push rbp          ; Save the base pointer
    mov rbp, rsp     ; Set up a stack frame

    ; --- Prepare the system call arguments ---
    mov byte [char_buffer], dil  ; Store the character (in dil) into the buffer

    mov rax, 1          ; System call number for write (sys_write)
    mov rdi, 1          ; File descriptor 1 (stdout)
    mov rsi, char_buffer ; Address of the character buffer
    mov rdx, 1          ; Number of bytes to write (1 character)

    ; --- Perform the system call ---
    syscall             ; Execute the write system call

    ; --- Cleanup and Return ---
    ; rax already contains the number of bytes written (or -1 on error)

    mov rsp, rbp      ; Restore the stack pointer
    pop rbp           ; Restore the base pointer
    ret               ; Return