; 12-strpbrk.asm
; char *asm_strpbrk(const char *s, const char *accept);

section .text
    global asm_strpbrk

asm_strpbrk:
    push rbp
    mov rbp, rsp

    ; Check for null pointers upfront
    test rdi, rdi  ; Check if s is NULL
    jz .null_s
    test rsi, rsi  ; Check if accept is NULL
    jz .null_s

.outer_loop:
    mov al, byte [rdi]     ; Load a character
    test al, al          ; Check for null in s
    jz .null_s             ; If null, s has been fully scanned

    push rdi             ; Save rdi (outer loop pointer)
    push rsi             ; Save rsi (accept pointer)
    
    mov rsi, [rbp + 8]       ;reset rsi.

.inner_loop:
    mov bl, byte [rsi]     ; Load a char from accept
    test bl, bl          ; Check for null in accept
    jz .next_outer       ; If null, no match, sad

    cmp al, bl           ; Compare characters
    je .found            ; If match, return pointer

    inc rsi             ; Move to the next char in accept
    jmp .inner_loop     ; Continue inner loop

.found:
    ;restore registers
    pop rsi
    pop rdi
    mov rax, rdi         ; Set return value to current position
    jmp .done_success

.next_outer:
;restore registers
    pop rsi
    pop rdi
    inc rdi             ; Move to the next character in s
    jmp .outer_loop     ; Continue outer loop

.null_s:
    mov rax, 0          ; Return NULL
.done_success:

    mov rsp, rbp
    pop rbp
    ret
