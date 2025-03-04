; 1-strcmp.asm
; int asm_strcmp(const char *s1, const char *s2);

section .text
    global asm_strcmp

asm_strcmp:
    xor rcx, rcx        ; Initialize index to 0

.loop:
    mov al, byte [rdi + rcx]  ; Load byte from s1 - MUST use byte
    mov bl, byte [rsi + rcx]  ; Load byte from s2 - MUST use byte

    cmp al, bl         ; Compare characters
    jne .different     ; If different, jump

    test al, al        ; Check for null terminator
    jz .equal          ; If null, strings are equal

    inc rcx             ; Increment index
    jmp .loop           ; Continue loop

.different:
    movsx eax, al       ; Sign-extend al into eax
    movsx ebx, bl       ; Sign-extend bl into ebx
    sub eax, ebx       ; Calculate difference
    ret                 ; Return

.equal:
    xor eax, eax        ; Return 0 for equal strings
    ret                 ; Return
