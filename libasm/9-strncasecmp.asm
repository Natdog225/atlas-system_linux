; asm_strncasecmp.asm
; int asm_strncasecmp(const char *s1, const char *s2, size_t n);

section .text
    global asm_strncasecmp

asm_strncasecmp:
    push rbp
    mov rbp, rsp

    mov rcx, rdx        ; rcx = n (maximum chars to compare)

.loop:
    test rcx, rcx        ; Check if n == 0
    jz .equal           ; If n == 0, strings are equal up to n

    mov al, byte [rdi]  ; Load byte from s1
    mov bl, byte [rsi]  ; Load byte from s2

    ; Convert to lowercase
    cmp al, 'A'
    jl .check_bl
    cmp al, 'Z'
    jle .lower_al       ; Jump if al is in range ['A', 'Z']
    jmp .check_bl
.lower_al:
    add al, 32          ; Convert al to lowercase

.check_bl:
    cmp bl, 'A'
    jl .compare
    cmp bl, 'Z'
    jle .lower_bl       ;Jump if bl is in range ['A', 'Z']
    jmp .compare
.lower_bl:
    add bl, 32          ; Convert bl to lowercase

.compare:
    cmp al, bl         ; Compare lowercase bytes
    jne .diff           ; If not equal, jump to .diff

    test al, al        ; Check for null terminator (either string)
    jz .equal          ; If null, strings are equal up to this point

    inc rdi             ; Move to next character in s1
    inc rsi             ; Move to next character in s2
    dec rcx             ; Decrement
    jmp .loop           ; Continue

.diff:
    ; Return the difference (al - bl)
    sub al, bl          ; Subtract bytes (8-bit subtraction)
    movsx eax, al       ; Sign-extend al to eax (32-bit result)
    jmp .done

.equal:
    ; Strings are equal up to n (or up to terminator).
    xor eax, eax        ; Return 0
    ; Fall through to .done

.done:
    mov rsp, rbp    ; Restore stack frame
    pop rbp         ;restore rbp
    ret