; 4-strstr.asm
; char *asm_strstr(const char *haystack, const char *needle);

section .text
    global asm_strstr

asm_strstr:
    push rbp
    mov rbp, rsp

    ; --- Check for Empty Needle ---
    mov al, byte [rsi] ; Load the first byte of the needle
    test al, al      ; Check if it's 0 (empty string)
    jz .empty_needle  ; If it's 0, jump to .empty_needle

    ; --- Setup (Simplified) ---
    mov r8, rdi         ; Save the original haystack pointer

.outer_loop:
    mov cl, byte [rdi]   ; Load a byte from haystack
    cmp cl, 0            ; Check for end of haystack
    je .not_found        ; If end of haystack, not found

    cmp cl, byte [rsi]   ; Compare with first byte of needle
    jne .next_haystack   ; If not equal, try next haystack char

    ; --- Potential match: Inner loop (Inlined) ---
    push rdi            ; Save haystack pointer
    push rsi            ; Save needle pointer
    push r9             ; Save inner loop counter

    xor r9, r9         ; Initialize inner loop counter

.inner_loop:  ;
    ; rdi: Current haystack location
    ; rsi: Current needle location
    ; r9:  Offset

    mov al, byte [rdi + r9] ; Load byte from haystack at offset
    mov bl, byte [rsi + r9] ; Load byte from needle at offset

    test bl, bl          ; Check for end of NEEDLE
    jz .match_found      ; If end of needle, it's a match!

    cmp al, bl           ; Compare haystack and needle bytes
    jne .no_match_inline ; If not equal, no match

    test al, al         ;check for the end of hay
    jz .no_match_inline

    inc r9              ; Increment the offset
    jmp .inner_loop     ; Continue comparing (inlined loop)

.match_found:          ; Jump target for inner loop match
    mov rax, 1          ; Set rax to 1 (match)
    jmp .found_continue ; Skip the no_match logic

.no_match_inline:      ; Jump target for inner loop mismatch
    xor rax, rax        ; Set rax to 0 (no match)

.found_continue:          ; Common exit point for inner loop
    pop r9             ; Restore inner loop counter
    pop rsi             ; Restore needle pointer
    pop rdi             ; Restore haystack pointer

    test rax, rax        ;check the match flag (1 for match, 0 otherwise)
    jnz .found          ; Jump if match
; else, falls through to the next outer loop

.next_haystack:
    inc rdi             ; Move to next character in haystack
    jmp .outer_loop     ; Continue outer loop

.found:
    ; Calculate return address (original haystack + offset)
    add r8, r9          ; Add the offset to the original haystack pointer
    mov rax, r8          ; Store the result in rax
    mov rsp, rbp
    pop rbp
    ret

.not_found:
    xor rax, rax        ; Set rax to 0 (NULL)
    mov rsp, rbp
    pop rbp
    ret

.empty_needle:
    mov rax, r8          ; Return the original haystack pointer
    mov rsp, rbp
    pop rbp
    ret