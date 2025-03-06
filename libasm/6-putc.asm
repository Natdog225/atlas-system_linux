; 6-putc.asm
; size_t asm_putc(int c);

section .data
    char_buffer db 0   ; Single-byte buffer

section .text
    global asm_putc
    global asm_write_char  ; Make the write function globally accessible

; This function performs the actual system call.
asm_write_char:
    push rbp              ; Preserve rbp
    mov rbp, rsp          ; Set up stack Frame
    mov rax, 1          ; Syscall number for write
    mov rdi, 1          ; stdout file descriptor
    mov rsi, [rbp + 16] ; Load buffer address from the stack
    mov rdx, 1          ; Number of bytes to write (1)
    syscall             ; Perform the write
    mov rsp, rbp       ; Restore Stack
    pop rbp            ; Restore rbp
    ret                 ; Return (to the C caller!)

asm_putc:
    push rbp           ; Save caller's base pointer.
    mov rbp, rsp       ; set up stack frame
    sub rsp, 8    ; Align stack to 16 bytes (and make space for char).

    mov byte [rel char_buffer], dil  ; Store the character (RIP-relative)

    ; Prepare for the jump to asm_write_char
    lea rax, [rel char_buffer]   ; Get the address of the buffer
    push rax                 ;push buffer to the stack.

    ; Jump to the write function.
    jmp asm_write_char      ; Jump (NOT call)

    ; Execution continues *here* after asm_write_char returns.

    add rsp, 8             ; Restore stack.
    mov rsp, rbp           ; restore stack pointer
    pop rbp            ; Restore base pointer.
    ret               ; Return to C caller.