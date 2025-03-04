; 1-strcmp.asm
; int asm_strcmp(const char *s1, const char *s2);

section .text
    global asm_strcmp

asm_strcmp:
    xor rcx, rcx        ; Initialize index to 0.  Use rcx (64-bit)

.loop:
    mov al, [rdi + rcx]  ; Load byte from s1.  Correct addressing.
    mov bl, [rsi + rcx]  ; Load byte from s2.  Correct addressing.

    cmp al, bl         ; Compare the bytes
    jne .diff           ; If they're different, jump to .diff

    test al, al        ; Check if al is 0 (end of string)
    jz .equal          ; If both are 0, strings are equal

    inc rcx             ; Increment index (64-bit)
    jmp .loop           ; Continue loop

.diff:
    sub eax, ebx       ; Calculate difference (al - bl)
                       ; Result is sign-extended to eax
    ret                 ; Return the difference

.equal:
    xor eax, eax        ; Set eax to 0 (strings are equal)
    ret                ; Return 0
	