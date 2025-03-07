; 7-puts.asm
; size_t asm_puts(const char *str);

section .text
    global asm_puts
    extern asm_strlen

asm_puts:
    push rbp
    mov rbp, rsp

    ; --- Calculate String Length ---
    push rdi          ; Save original string pointer 
    call asm_strlen   ; Get string length (result in rax)
    pop rdi           ; Restore original string pointer

    ; --- Allocate Stack Space and Align ---
    mov rdx, rax     ; rdx = string length
    inc rdx          ; rdx = string length + 1 (for newline)
    push rdx          ; Save the length + 1
    ; sub rsp, rax   ; Allocate space for string.
    ; sub rsp, 8     ; Allocate space for '\n',
    add rdx, 15
    and rdx, -16   ; clearing the last 4 bits, rdx % 16 == 0.
    sub rsp, rdx ;allocate stack space, and ensure is 16 aligned.

    ; --- Copy String to Stack (using rep movsb) ---
    ;   rdi: source (original string)
    ;   rsi: destination (stack)
    ;   rcx: count (string length)
    mov rsi, rdi          ; Source = original string
    mov rdi, rsp          ; Destination = stack
    mov rcx, rax          ; Number of bytes to copy = string length
    rep movsb             ; Copy the string to the stack

    ; --- Add Newline ---
    mov byte [rdi], 10     ; rsp now points *past* the copied string,

    ; --- Prepare for syscall ---
    mov rax, 1           ; Syscall number for write
    mov rdi, 1           ; File descriptor 1 (stdout)
    ; rsi is already set to rsp (beginning of copied string on stack)
    ; rdx was string_len+1, so it contains the correct count.
    pop rdx             ; recover string length + 1

    ; --- Perform the system call ---
    syscall              ; Write to stdout

    ; --- Cleanup and Return ---
    ; add rsp, <amount> ; No longer add rsp manually - calculate the space

    mov rsp, rbp      ; Full stack frame restoration is simpler
    pop rbp           ; and avoids any potential alignment issues.
    ret                ; Return
