; asm_strspn.asm
; size_t asm_strspn(const char *s, const char *accept);

section .text
    global asm_strspn

asm_strspn:
    push rbp
    mov rbp, rsp
    ; Use rcx as the counter
    xor rcx, rcx  ; Initialize the count (rcx) to 0

.outer_loop:
    mov al, byte [rdi + rcx] ; Load char from 's'
    test al, al          ; Check for null terminator in 's'
    jz .done             ; If null terminator, return the count

    push rdi  ; Save rdi.
    push rcx  ; Save rcx.

    ; Inner loop: Scan 'accept' string for a match
    mov rdi, rsi        ; rdi = accept string (inner loop)

.inner_loop:
    mov bl, byte [rdi]  ; Load char from 'accept'
    test bl, bl          ; Check for null terminator in 'accept'
    jz .not_found       ; If null terminator, char not found

    cmp al, bl           ; Compare s[i] with accept[j]
    je .found_match     ; If match, increment count and continue outer loop

    inc rdi             ; Move to next char in 'accept'
    jmp .inner_loop     ; Check next char in accept

.found_match:
    ; Character found in accept string.
    inc rcx           ; Increment
    pop rcx         ; Restore
    pop rdi
    jmp .outer_loop  ; continue next character in 's'

.not_found:
    ; Character not found in accept string.
    pop rcx ;restore registers.
    pop rdi
    ; Fall through to .done

.done:
     mov rax, rcx    ; The count is the return, put it on rax.
    mov rsp, rbp
    pop rbp
    ret
