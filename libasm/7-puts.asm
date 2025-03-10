; 7-puts.asm 
; size_t asm_puts(const char *str);

section .text
    global asm_puts
    extern asm_strlen

asm_puts:
    push rbp
    mov rbp, rsp
    push rdi
    call asm_strlen
    pop rdi
    mov rdx, rax  ; rdx = string length (NO +1)
    push rdx      ; Save string length (NOT +1)
    add rdx, 15
    and rdx, -16
    sub rsp, rdx  ; Allocate aligned stack space
    mov rsi, rdi
    mov rdi, rsp
    mov rcx, rax  ; Copy ONLY the string (no newline space)
    rep movsb

    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    pop rdx        ; Get back the ORIGINAL string length
    ; DO NOT increment rdx here!

    syscall

    mov rsp, rbp
    pop rbp
    ret
