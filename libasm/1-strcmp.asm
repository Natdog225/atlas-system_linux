section .text
    global asm_strcmp

asm_strcmp:
    xor rcx, rcx        ; Initialize index to 0

.loop:
    mov al, byte [rdi + rcx]  ; Load byte from s1
    mov bl, byte [rsi + rcx]  ; Load byte from s2

    cmp al, bl         ; Compare characters
    jne .different     ; If different, jump

    test al, al        ; Check for null terminator (in *either* string)
    jz .equal          ; If *either* is null, and they're equal, we're done

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
