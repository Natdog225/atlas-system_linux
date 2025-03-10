; 7-puts.asm
; size_t asm_puts(const char *str);

section .text
    global asm_puts
    extern asm_strlen

asm_puts:
    push rbp
    mov rbp, rsp

    ; Save registers
    push rdi        ; Save the string pointer
    push rsi
    push rdx
    push rcx

    call asm_strlen   ; Result in rax

    ; Restore registers
    pop rcx
    pop rdx
    pop rsi
    pop rdi    ; Original string pointer

    mov rdx, rax     ; rdx = string length
    inc rdx          ; rdx = string length + 1 (for newline)
    push rdx        ; save for later

    ; Align stack, we have rbp + 4 registers = 40 bytes
    add rdx, 15
    and rdx, -16
    sub rsp, rdx     ; Allocate aligned space on the stack.

    ; --- Copy String to Stack (using rep movsb) ---
    mov rsi, [rbp + 40] ; Source = original string
    mov rdi, rsp      ; Destination = stack
    mov rcx, rax      ; Number of bytes to copy = string length
    rep movsb         ; Copy the string.

    ; --- Add Newline ---
    mov byte [rdi], 10

    ; --- Prepare for syscall ---
    mov rax, 1           ; Syscall number for write
    mov rdi, 1           ; File descriptor 1 (stdout)
    mov rsi, rsp        ; Pointer to string on stack
    pop rdx            ; String length + 1

    ; --- Perform the system call ---
    syscall              ; Write to stdout

    ; --- Cleanup and Return ---
    ; rax contains the number of bytes written

    mov rsp, rbp      ; deallocate stack frame
    pop rbp          ; restore rbp
    ret              ; Return (with byte count in rax)
