section .text
global asm_strncasecmp
asm_strncasecmp:
    ; Save registers that must be preserved according to x86_64 System V AMD64 ABI
    push rbp
    mov rbp, rsp
    push rbx        ; Must preserve rbx
    
    mov rcx, rdx    ; rcx = n (counter)
    
.loop:
    test rcx, rcx    ; Check if n == 0
    jz .equal       ; If n == 0, strings are equal up to n
    
    mov al, byte [rdi]  ; Load byte from s1
    mov bl, byte [rsi]  ; Load byte from s2
    
    ; If it hits null in either string, they're not equal
    test al, al
    jz .null_s1
    test bl, bl
    jz .null_s2
    
    ; Convert al to lowercase if needed
    cmp al, 'A'
    jl .check_bl
    cmp al, 'Z'
    jg .check_bl
    add al, 32
    
.check_bl:
    ; Convert bl to lowercase if needed
    cmp bl, 'A'
    jl .compare
    cmp bl, 'Z'
    jg .compare
    add bl, 32
    
.compare:
    cmp al, bl      ; Compare the bytes
    jne .diff       ; return difference
    
    inc rdi         ; Move to next character in s1
    inc rsi         ; Move to next character in s2
    dec rcx         ; Decrement counter
    jmp .loop       ; Continue loop

.null_s1:
    ; s1 is null, s2 has more characters
    mov al, 0
    mov bl, byte [rsi]
    sub al, bl      ; Will be negative
    movsx eax, al
    jmp .done

.null_s2:
    ; s2 is null, s1 has more characters
    mov al, byte [rdi]
    mov bl, 0
    sub al, bl      ; Will be positive
    movsx eax, al
    jmp .done

.diff:
    ; Return the difference (al - bl)
    sub al, bl      ; Calculate difference
    movsx eax, al   ; Sign-extend to 32 bits
    jmp .done

.equal:
    xor eax, eax    ; Return 0 yay equality!
    
.done:
    ; Restore saved registers
    pop rbx
    mov rsp, rbp
    pop rbp
    ret
