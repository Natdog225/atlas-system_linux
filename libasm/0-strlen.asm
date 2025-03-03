; 0-strlen.asm
; size_t asm_strlen(const char *str);

section .text
    global asm_strlen

asm_strlen:
    xor rax, rax        ; Initialize counter (rax) to 0

.loop:
    cmp byte [rdi + rax], 0  ; Compare the byte at (str + rax) with 0
    je .end             ; If it's 0 (end of string), jump to .end
    inc rax             ; Increment the counter
    jmp .loop           ; Jump back to the beginning of the loop

.end:
    ret                 ; Return (rax contains the length)
