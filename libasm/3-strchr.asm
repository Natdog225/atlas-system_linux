; 3-strchr.asm
; char *asm_strchr(const char *s, int c);

section .text
    global asm_strchr

asm_strchr:
    xor rax, rax     ; rax will be our index

.loop:
    mov cl, [rdi + rax]    ; Get a char from string
    cmp cl, 0           ; Null terminator?
    je .not_found

    cmp cl, sil          ; Compare char with c
    je .found

    inc rax
    jmp .loop


.found:
    lea rax, [rdi + rax]  ; Character found, set return value
    ret

.not_found:
   xor rax, rax  ;Set rax to 0 (NULL)
   ret
