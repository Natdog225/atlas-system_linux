; 1-strcmp.asm
; int asm_strcmp(const char *s1, const char *s2);

section .text
    global asm_strcmp

asm_strcmp:
    xor rcx, rcx        ; Initialize index to 0

.loop:
    mov al, [rdi + rcx]  ; Load byte from s1
    mov bl, [rsi + rcx]  ; Load byte from s2

    cmp al, bl         ; Compare the bytes
    jne .diff           ; If they're different, jump to .diff

    test al, al        ; Check if al is 0 (end of string)
    jz .equal          ; If both are 0 (and we haven't jumped to .diff), strings are equal

    inc rcx             ; Increment index
    jmp .loop           ; Continue loop

.diff:
    sub eax, ebx      ; Calculate difference (al - bl, sign-extended)
    ret                 ; Return the difference

.equal:
    xor eax, eax        ; Set eax to 0 (strings are equal)
    ret                ; Return 0
