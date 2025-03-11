section .text
global asm_strspn
asm_strspn:
    ; Save registers
    push rbp
    mov rbp, rsp
    push rbx
    xor rax, rax    ; Initialize return value to 0

.loop:
    mov al, byte [rdi]  ; Load character from s
    test al, al        ; Check for null terminator
    jz .done          ; If null, we're done

    ; Scan accept string to find match
    mov rsi, rdx      ; Reset rsi to start of accept string
.scan_accept:
    mov bl, byte [rsi] ; Load character from accept
    test bl, bl       ; Check for null terminator
    jz .next_char    ; If null,no match, bit sad innit 
    
    cmp al, bl       ; Compare characters
    je .found_match  ; If equal, we found a match
    
    inc rsi          ; Move to next character in accept
    jmp .scan_accept ; Continue

.found_match:
    inc rax          ; Increment match
    inc rdi          ; Move to next character
    jmp .loop        ; Continue

.next_char:
    xor rax, rax    ; Reset count to 0
    jmp .done       ; Exit with 0

.done:
    ; Restore saved registers
    pop rbx
    mov rsp, rbp
    pop rbp
    ret
