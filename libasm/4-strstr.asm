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

.inner_loop:  ; No longer a separate function, inlined here
    ; rdi: Current haystack location
    ; rsi: Current needle location
    ; r9:  Offset

    mov al, byte [rdi + r9] ; Load byte from haystack at offset
    mov bl, byte [rsi + r9] ; Load byte from needle at offset

    test bl, bl          ; Check for end of NEEDLE
    jz .match_found      ; If end of needle, it's a match! WOO

    cmp al, bl           ; Compare haystack and needle bytes
    jne .no_match_