; asm_strcspn.asm
; size_t asm_strcspn(const char *s, const char *reject);

section .text
    global asm_strcspn

asm_strcspn:
    push rbp
    mov rbp, rsp

    xor rcx, rcx  ; Initialize count (rcx) to 0

.outer_loop:
    mov al, byte [rdi + rcx] ; Load char from 's'
    test al, al          ; Check for null
    jz .done             ; If null, return the count

    push rdi  ; Save rdi.
    push rsi  ; Save rsi
    push rcx  ; Save rcx.

    ; Inner loop: Scan 'reject' string for a match
    mov rdi, rsi        ; rdi = reject string (inner loop)
    xor rbx, rbx        ; rbx is the inner loop counter

.inner_loop:
    mov bl, byte [rdi + rbx]  ; Load char from 'reject'
    test bl, bl          ; Check for null in 'reject'
    jz .not_found       ; If null, char not found

    cmp al, bl           ; Compare s[i] with reject[j]
    je .found_reject     ; If match, return current count

    inc rbx             ; Move to the next char
    jmp .inner_loop     ; Check the next char

.found_reject:
    ; Character FROM s found in reject string.
    pop rcx ;restore registers.
    pop rsi
    pop rdi
    jmp .done           ; Return current count

.not_found:
    ; Character not found in reject string
    pop rcx ;restore registers.
    pop rsi
    pop rdi
    inc rcx           ; Increment the count
    jmp .outer_loop    ; Check next character

.done:
    mov rax, rcx        ; Move the count into rax (return value)
    mov rsp, rbp        ; Restore Stack
    pop rbp             ; Restore rbp
    ret
