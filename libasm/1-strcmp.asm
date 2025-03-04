; 1-strcmp.asm
; int asm_strcmp(const char *s1, const char *s2);

section .text
    global asm_strcmp
    BITS 64                  ; Specify 64-bit mode

asm_strcmp:
    xor rcx, rcx            ; Initialize counter to 0 (full 64-bit register)

.loop:
    mov al, byte [rdi + rcx]    ; Load byte from s1
    mov bl, byte [rsi + rcx]    ; Load byte from s2

    cmp al, bl               ; Compare characters
    jne .different          ; If different, exit loop

    test al, al             ; Check for null terminator
    je .equal              ; If null, strings are equal

    inc rcx                 ; Increment index
    jmp .loop              ; Continue loop

.different:
    movsx eax, al          ; Sign-extend al into eax
    movsx ebx, bl          ; Sign-extend bl into ebx
    sub eax, ebx           ; Calculate difference
    ret                    ; Return difference

.equal:
    xor eax, eax           ; Return 0 for equal strings
    ret                    ; Return
