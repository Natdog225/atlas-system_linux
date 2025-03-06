; 4-strstr.asm
; char *asm_strstr(const char *haystack, const char *needle);

section .text
    global asm_strstr
    extern asm_strlen

asm_strstr:
    ; --- Handle empty needle case ---
    push rbp
    mov rbp, rsp

    mov rdi, rsi     ; Put the address of 'needle' in rdi (for asm_strlen)
    call asm_strlen ; get the length of needle.
    test rax, rax   ; check if needle length is 0.
    jz .empty_needle ; Jump if zero.

    ; --- Setup ---
    mov rdx, rax ; store the length of needle to rdx.
    mov rsi, [rbp + 16] ; restore rsi to the correct address, pointing to needle.
    mov r8, rdi  ; save the hay pointer, before it changes.

.outer_loop:
    mov cl, byte [rdi]   ; Load a byte from haystack
    cmp cl, 0            ; Check for end of haystack
    je .not_found        ; If end of haystack, not found

    cmp cl, byte [rsi]   ; Compare with first byte of needle
    jne .next_haystack   ; If not equal, try next haystack char

    ; --- Potential match: Inner loop ---
    push rdi            ; Save haystack pointer
    push rsi            ; Save needle pointer
    push rcx       ;Save outer loop counter
    push rdx          ;Save needle len
    
    mov rcx, 1      ; start at 1 since we already know first char matches.
    call .inner_loop    ; Call inner loop

    pop rdx            ; Restore registers
    pop rcx
    pop rsi
    pop rdi

    test rax, rax   ; Check if inner_loop found a match
    jnz .found       ; If match found, jump to .found

.next_haystack:
    inc rdi             ; Move to next character in haystack
    jmp .outer_loop     ; Continue outer loop

.inner_loop:
;Inputs:
; rdi: Current haystack location
; rsi: Needle address.
; rcx: index.
; rdx: needle length
;Return
; rax: 1 if match, 0 otherwise.
.inner_compare:

    cmp rcx, rdx ; compare counter with needle length
    jge .match ;If all chars checked, return

    mov al, byte [rdi + rcx] ; get next haystack char.
    mov bl, byte [rsi + rcx]  ;get next needle char

    cmp al, bl
    jne .no_match

    inc rcx     ; increase index
    jmp .inner_compare
.match:
    mov rax, 1 ;set return to 1, indicating match
    ret
.no_match:
    xor rax, rax    ;Set rax to zero
    ret

.found:
    ; Calculate the address to return (original haystack + index)
    sub rdi, r8          ;Calculate how much we advanced
    sub rdi, 1         ;Remove 1, since the first char matches.
    add r8, rdi
    mov rax, r8   ; Address of matching substring
    mov rsp, rbp    ; restore stack
    pop rbp
    ret                 ; Return

.not_found:
    xor rax, rax        ; Set rax to 0
    mov rsp, rbp    ; restore stack
    pop rbp
    ret                 ; Return NULL

.empty_needle:
    mov rax, [rbp + 8]        ; Return haystack if needle is empty bit sad innit 
    mov rsp, rbp    ; restore stack
    pop rbp
    ret
