; 1-strcmp.asm
; int asm_strcmp(const char *s1, const char *s2);

section .text
    global asm_strcmp
    BITS 64                  ;

asm_strcmp:
    push rbp                ; Save base pointer
    mov rbp, rsp           ; Set up frame pointer
    push rbx               ; Save callee-saved register
    
    xor ecx, ecx           ; Initialize counter to 0
    mov rsi, rsi           ; Second string pointer already in rsi
    mov rdi, rdi           ; First string pointer already in rdi

.loop:
    mov al, byte [rdi + rcx]    ; Load byte from s1
    mov bl, byte [rsi + rcx]    ; Load byte from s2

    ; Check if we've reached the end of either string
    test al, al             ; Check if s1 is null
    jz .s1_null           ; If s1 is null
    test bl, bl            ; Check if s2 is null
    jz .s2_null           ; If s2 is null

    cmp al, bl             ; Compare characters
    jne .different        ; If different, exit loop

    inc rcx                ; Increment index
    jmp .loop             ; Continue loop

.s1_null:
    test bl, bl           ; Check if s2 is also null
    jz .equal            ; If both null, strings are equal
    mov al, 0            ; s1 is null but s2 isn't
    mov bl, [rsi + rcx]  ; Get current s2 character
    jmp .different       ; Compare null with character

.s2_null:
    mov al, [rdi + rcx]  ; Get current s1 character
    mov bl, 0            ; s2 is null but s1 isn't
    jmp .different       ; Compare character with null

.different:
    xor ah, ah           ; Clear high byte of eax
    xor bh, bh           ; Clear high byte of ebx
    movsx eax, al        ; Sign-extend al into eax
    movsx ebx, bl        ; Sign-extend bl into ebx
    sub eax, ebx         ; Calculate difference
    jmp .exit

.equal:
    xor eax, eax         ; Return 0 for equal strings
    jmp .exit

.exit:
    pop rbx              ; Restore saved register
    mov rsp, rbp         ; Restore stack pointer
    pop rbp              ; Restore base pointer
    ret                  ; Return to caller
