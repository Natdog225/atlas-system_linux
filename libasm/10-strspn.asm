SECTION .text
global asm_strspn

asm_strspn:
    push rbx
    mov rcx, -1 ; next_char counter
    mov r8, 0   ; result

    next_char:
        inc rcx
        movzx rax, byte [rdi + rcx]

        test rax, rax
        jz end_of_cmp

        mov rdx, -1 ; reset check_char counter
        jmp check_char
        cont_next_char:
        inc r8

        jmp next_char

    check_char:
        inc rdx
        movzx rbx, byte [rsi + rdx]

        test rbx, rbx
        jz end_of_cmp

        cmp rax, rbx
        je cont_next_char

        jmp check_char

    ; RETURN
    end_of_cmp:
        pop rbx
        mov rax, r8
        ret
