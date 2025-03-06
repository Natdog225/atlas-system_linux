; 4-strstr.asm
; char *asm_strstr(const char *haystack, const char *needle);

section .text
    global asm_strstr

asm_strstr:
    push rbp
    mov rbp, rsp

    ; --- Check for Empty Needle ---
    cmp byte [rsi], 0  ; Directly check if needle is empty
    je .empty_needle   ; If needle is empty, return haystack

    ; --- Setup (Simplified) ---
    mov r8, rdi         ; Save the original haystack pointer

.outer_loop:
    mov cl, byte [rdi]   ; Load a byte from haystack
    cmp cl, 0            ; Check for end of haystack
    je .not_found        ; If end of haystack, not found

    cmp cl, byte [rsi]   ; Compare with first byte of needle
    jne .next_haystack   ; If not equal, try next haystack char

    ; --- Potential match: Inner loop ---
    push rdi            ; Save haystack pointer
    push rsi            ; Save needle pointer

    call .inner_loop    ; Call inner loop

    ; No need to pop rcx or rdx

    pop rsi             ; Restore needle pointer
    pop rdi             ; Restore haystack pointer

    test rax, rax   ; Check if inner_loop found a match
    jnz .found       ; If match found, jump to .found

.next_haystack:
    inc rdi             ; Move to next character in haystack
    jmp .outer_loop     ; Continue outer loop

.inner_loop:
    ; rdi: Current haystack location
    ; rsi: Current needle location
    ; Return: rax = 1 if match, 0 otherwise

    ; Initialize a counter (we *could* use rcx, but a separate register is cleaner)
    ;  We don't strictly *need* a counter, but it makes the return address calculation
    ;  slightly easier to understand, as it directly represents the offset.
    push r9 ; using r9 as inner loop counter. Must be saved.
    xor r9, r9         ;  r9 = 0

.inner_compare:
    mov al, byte [rdi + r9] ; Load byte from haystack
    mov bl, byte [rsi + r9] ; Load byte from needle

    cmp bl, 0            ; *** Check for end of NEEDLE ***
    je .match           ; If end of needle, it's a match YAY

    cmp al, bl           ; Compare haystack and needle bytes
    jne .no_match       ; If not equal, no match

    cmp al, 0           ;check if it is the end of the hay.
    je .no_match

    inc r9              ; Increment the counter
    jmp .inner_compare  ; Continue comparing

.match:
    mov rax, 1          ; Set rax to 1 (match found)
    pop r9
    ret

.no_match:
    xor rax, rax        ; Set rax to 0 (no match)
    pop r9
    ret

.found:
    ; Calculate return address (original haystack + offset)
    add r8, r9          ; Add the offset (from r9) to the original haystack pointer
    mov rax, r8          ; Store the result in rax
    mov rsp, rbp
    pop rbp
    ret

.not_found:
    xor rax, rax        ; Set rax to 0
    mov rsp, rbp
    pop rbp
    ret

.empty_needle:
    mov rax, r8          ; Return the original haystack pointer
    mov rsp, rbp
    pop rbp
    ret
