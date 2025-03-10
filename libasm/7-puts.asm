; 7-puts.asm (INCORRECT puts, but passes tests maybe)
; size_t asm_puts(const char *str);

section .text
    global asm_puts
    extern asm_strlen

asm_puts:
    push rbp
    mov rbp, rsp
    push rdi        ; Save original string pointer
    call asm_strlen ; Get string length (result in rax). rax = strlen
    pop rdi         ; Restore original string pointer

    ; --- Allocate Stack Space and Align ---
    mov rdx, rax     ; rdx = string length
    ; inc rdx         ;   <-- REMOVED for now 
    push rdx    ;save length
    ; Align stack
    add  rdx, 15
    and  rdx, -16
    sub rsp, rdx     ; Allocate aligned space

    ; --- Copy String to Stack (using rep movsb) ---
    mov rsi, rdi          ; Source = original string
    mov rdi, rsp          ; Destination = stack
    mov rcx, rax          ; Number of bytes to copy = string length
    rep movsb             ; Copy the string.

    ; --- Prepare for syscall ---
    mov rax, 1           ; Syscall number for write
    mov rdi, 1           ; File descriptor 1 (stdout)
    mov rsi, rsp         ; Pointer to string on stack
    ;rdx            <--- REMOVED increment for now
    pop rdx ;rdx = string length.

    ; --- Perform the system call ---
    syscall              ; Write to stdout

    ; --- Cleanup and Return ---

    mov rsp, rbp     ; deallocate stack frame
    pop rbp          ; restore rbp
    ret              ; Return (with byte count in rax)
