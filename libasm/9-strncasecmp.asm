section .text
global asm_strncasecmp
asm_strncasecmp:
    ; Save registers
    push rbp
    mov rbp, rsp
    push rbx        ;preserve rbx
    
    mov rcx, rdx    ; rcx = n (counter)
    
.loop:
    test rcx, rcx    ; Check if n == 0
    jz .equal       ; If n == 0, strings are equal up to n
    
    mov al, byte [rdi]  ; Load byte from s1
    mov bl, byte [rsi]  ; Load byte from s2
    
    ; Convert al to lowercase if needed
    cmp al, 'A'
    jl .check_bl
    cmp al, 'Z'
    jg .check_bl
    add al, 32      ; Convert to lowercase
    
.check_bl:
    ; Convert bl to lowercase if needed
    cmp bl, 'A'
    jl .compare
    cmp bl, 'Z'
    jg .compare
    add bl, 32      ; Convert to lowercase
    
.compare:
    cmp al, bl      ; Compare bytes
    jne .diff       ; If not equal, return difference, ew
    
    ; Check for null terminator in either string
    test al, al     ; Check if we hit end of s1
    jz .equal       ; If null, strings are equal up to this point
    test bl, bl     ; Check if we hit end of s2
    jz .equal       ; If null, strings are equal up to this point
    
    inc rdi         ; Move to next char in s1
    inc rsi         ; Move to next char in s2
    dec rcx         ; Decrement
    jmp .loop       ; Continue

.diff:
    ; Return the difference (al - bl)
    sub al, bl      ; Calculate difference
    movsx eax, al   ; Sign-extend to 32 bits
    jmp .done

.equal:
    xor eax, eax    ; Return 0 (strings are equal)
    
.done:
    ; Restore saved registers
    pop rbx
    mov rsp, rbp
    pop rbp
    ret