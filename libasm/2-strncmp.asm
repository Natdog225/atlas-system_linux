; 2-strncmp.asm
; int asm_strncmp(const char *s1, const char *s2, size_t n);

section .text
    global asm_strncmp

asm_strncmp:
    xor rcx, rcx        ; Initialize index (rcx) to 0

.loop:
    cmp rcx, rdx        ; Compare index with n
    jge .equal          ; If index >= n, strings are equal up to n

    mov al, [rdi + rcx]  ; Load byte from s1
    mov bl, [rsi + rcx]  ; Load byte from s2

    cmp al, bl         ; Compare the bytes
    jne .diff          ; If different, jump to .diff

    test al, al        ; Check for null terminator
    jz .equal          ; If both are 0, they're equal up to this point

    inc rcx             ; Increment index
    jmp .loop           ; Continue loop

.diff:
    sub eax, ebx      ; Calculate difference
    ret

.equal:
    xor eax, eax        ; Set return value to 0 (equal)
    ret
