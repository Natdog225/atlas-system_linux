; 4-strstr.asm
; char *asm_strstr(const char *haystack, const char *needle);

section .text
    global asm_strstr

asm_strstr:
    ; Handle empty needle case directly (always found)
    cmp byte [rsi], 0
    je .empty_needle

.outer_loop:
    mov al, [rdi]       ; Load character from haystack
    cmp al, 0           ; Check for end of haystack
    je .not_found

    cmp al, [rsi]       ; Compare first character of haystack and needle
    jne .next_haystack_char ; If not equal, move to the next char in haystack

    ; Potential match: Inner loop to compare the rest
    push rdi            ; Save haystack pointer
    push rsi            ; Save needle pointer
    call .inner_loop    ; call inner_loop to avoid using call instruction
    pop rsi            ; Restore needle pointer
    pop rdi            ; Restore haystack pointer

    test rax, rax   ;Check the result of compare_rest
    jnz .found     ; If result is not zero, we have a match, it's a STRING! Congratulations


.next_haystack_char:
    inc rdi             ; Move to next character in haystack
    jmp .outer_loop     ; Continue outer loop

.inner_loop:
    xor rcx, rcx ; index counter
.compare_loop:

    mov al, [rdi + rcx]     ; Load byte from haystack
    mov bl, [rsi + rcx]    ;Load byte from needle
    cmp bl, 0       ; Check for null terminator in needle
    je .found_inner

    cmp al, bl
    jne .not_found_inner

    inc rcx
    jmp .compare_loop
.not_found_inner:
    xor rax, rax
    ret

.found_inner:
     lea rax, [rdi + rcx]  ; Calculate address of match within haystack.
     sub rax, rcx
     ret
.empty_needle:
    mov rax, rdi        ; If needle is empty, return haystack
    ret

.found:

    ret

.not_found:
    xor rax, rax        ; Set rax to 0 (NULL - not found)
    ret
