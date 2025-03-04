; 1-strcmp.asm
; int asm_strcmp(const char *s1, const char *s2);

section .text
    global asm_strcmp
    BITS 64                  ; Specify 64-bit mode

asm_strcmp:
    push rbp                ; Save base pointer
    mov rbp, rsp           ; Set up frame pointer
    push rbx               ; Save callee-saved register
    
    xor ecx, ecx           ; Initialize counter to 0
    mov rsi, rsi           ; Second string pointer already in rsi
    mov rdi, rdi           ; First string pointer already in rdi

.loop:
    mov al, byte [rdi + rcx]    ; Load byte from s1
    mov bl, byte [rsi + rcx]    ; Load byte from s2

    cmp al, bl               ; Compare characters
    jne .different          ; If different, exit loop

    test al, al             ; Check for null terminator
    je .equal              ; If null, strings are equal

    inc rcx                 ; Move to next character
    jmp .loop              ; Continue comparison

.different:
    xor ah, ah             ; Clear high byte of eax
    xor bh, bh             ; Clear high byte of ebx
    movzx eax, al          ; Sign-extend al into eax
    movzx ebx, bl          ; Sign-extend bl into ebx
    sub eax, ebx           ; Calculate difference
    jmp .exit

.equal:
    xor eax, eax           ; Return 0 for equal strings
    jmp .exit

.exit:
    pop rbx                ; Restore callee-saved register
    mov rsp, rbp          ; Restore stack pointer
    pop rbp               ; Restore base pointer
    ret                   ; Return to caller
