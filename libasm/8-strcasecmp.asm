section .text
global asm_strcasecmp

asm_strcasecmp:
    push rbp
    mov rbp, rsp
    
    ; Save registers to modify
    push rbx
    push r12
    push r13
    
    ; Initialize pointers
    mov r12, rdi    ; s1
    mov r13, rsi    ; s2
    
asm_strcasecmp.loop:
    ; Load characters, choose your fighter
    mov al, [r12]   ; Get char from s1
    mov bl, [r13]   ; Get char from s2
    
    ; Check for end of string
    test al, al     ; If s1 char is 0
    jz asm_strcasecmp.end_loop    ; We're done, yay!
    
    test bl, bl     ; If s2 char is 0
    jz asm_strcasecmp.end_loop    ; We're done, yay!
    
    ; Convert to lowercase
    mov cl, al      ; Save s1 char
    mov dl, bl      ; Save s2 char
    
    ; Convert s1 char to lowercase
    cmp cl, 'A'
    jl asm_strcasecmp.skip_s1
    cmp cl, 'Z'
    jg asm_strcasecmp.skip_s1
    add cl, 32      ; Convert to lowercase
    
asm_strcasecmp.skip_s1:
    cmp dl, 'A'
    jl asm_strcasecmp.skip_s2
    cmp dl, 'Z'
    jg asm_strcasecmp.skip_s2
    add dl, 32      ; Convert to lowercase

asm_strcasecmp.skip_s2:
    ; Compare characters, Fight!
    cmp cl, dl
    jne asm_strcasecmp.end_loop   ; If different, give up
    
    ; Move to next characters
    inc r12
    inc r13
    jmp asm_strcasecmp.loop

asm_strcasecmp.end_loop:
    ; If we got here, strings differ or are equal ideally. Maybe?
    ; Return difference between current characters
    mov cl, [r12]   ; Get current s1 char
    mov dl, [r13]   ; Get current s2 char
    
    ; Convert to lowercase for final comparison
    cmp cl, 'A'
    jl asm_strcasecmp.skip_final_s1
    cmp cl, 'Z'
    jg asm_strcasecmp.skip_final_s1
    add cl, 32

asm_strcasecmp.skip_final_s1:
    cmp dl, 'A'
    jl asm_strcasecmp.skip_final_s2
    cmp dl, 'Z'
    jg asm_strcasecmp.skip_final_s2
    add dl, 32

asm_strcasecmp.skip_final_s2:
    ; Return difference, Winner!
    sub cl, dl      ; cl - dl
    movsx ecx, cl   ; Sign extend to 32 bits
    mov eax, ecx    ; Return value in rax
    
asm_strcasecmp.cleanup:
    ; Restore registers
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret