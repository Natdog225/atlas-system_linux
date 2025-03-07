; 7-puts.asm
; size_t asm_puts(const char *str);

section .text
    global asm_puts
    extern asm_strlen

asm_puts:
    push rbp
    mov rbp, rsp
    sub rsp, 16  ; Allocate stack space (aligned)

    push rdi      ; Save the original string pointer
    call asm_strlen ; Get string length (result in rax)
    pop rdi       ; Restore original string pointer

    mov rdx, rax  ; rdx = string length
    inc rdx       ; rdx = string length + 1 (for newline)

    mov rcx, 0    ; Initialize copy loop counter

.copy_loop:
    cmp rcx, rdx  ; copied the entire original string?
    jae .add_newline ; Cool add newline
    mov bl, [rdi + rcx]   ; Load byte from input string
    mov [rsp + rcx], bl   ; Store byte on the stack
    inc rcx                ; Increment counter
    jmp .copy_loop         ; Repeat

.add_newline:
    mov byte [rsp + rcx], 10  ; Add newline character

    ; Prepare for the write syscall
    mov rax, 1          ; Syscall number for write
    mov rdi, 1          ; File descriptor 1 (stdout)
    mov rsi, rsp        ; Pointer to the string (on the stack)

    syscall            ; Perform the write

    add rsp, 16  ; Deallocate stack space
    pop rbp      ; Restore caller's base pointer
    ret           ; Return
