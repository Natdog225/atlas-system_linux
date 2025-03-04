; 1-strcmp.asm
; int asm_strcmp(const char *s1, const char *s2);

section .text
    global asm_strcmp
    BITS 64

asm_strcmp:
    push rbp
    mov rbp, rsp
    ; push rbx      ; Not strictly necessary, as we don't modify rbx

    xor rcx, rcx        ; Initialize counter to 0 (use rcx for 64-bit addressing)

.loop:
    mov al, byte [rdi + rcx]  ; Load byte from s1
    mov bl, byte [rsi + rcx]  ; Load byte from s2

    cmp al, bl         ; Compare characters
    jne .different     ; If different, exit loop

    test al, al        ; Check for null terminator (both strings)
    jz .equal          ; If null, strings are equal up to this point

    inc rcx             ; Move to next character
    jmp .loop           ; Continue comparison

.different:
    movsx eax, al       ; Sign-extend al into eax
    movsx ebx, bl       ; Sign-extend bl into ebx
    sub eax, ebx       ; Calculate difference (eax = al - bl)
    ; jmp .exit       ; No jmp!  Fall through to ret
    ret                 ; Return directly.

.equal:
    xor eax, eax        ; Return 0 for equal strings
    ret                ; Return.

;.exit:                ; Unnecessary label
;    pop rbx
;    mov rsp, rbp
;    pop rbp
;    ret
