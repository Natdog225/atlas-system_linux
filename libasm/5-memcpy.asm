; 5-memcpy.asm
; void *asm_memcpy(void *dest, const void *src, size_t n);

section .text
    global asm_memcpy

asm_memcpy:
    ; --- Setup ---
    push rbp          ; Save the base pointer
    mov rbp, rsp     ; Set up a stack frame
    mov rax, rdi     ; Store the destination pointer in rax

    ; --- Copy Loop ---
    ; Use rcx as a counter.
    mov rcx, rdx      ; Initialize the counter (rcx) with n

.copy_loop:
    cmp rcx, 0          ; Check if n (rcx) is 0
    je .done          ; If n is 0, we're done

    mov bl, byte [rsi]  ; Load a byte from the source
    mov byte [rdi], bl  ; Store the byte at the destination

    inc rsi             ; Increment the source pointer
    inc rdi             ; Increment the destination pointer
    dec rcx             ; Decrement the counter
    jmp .copy_loop      ; Repeat

.done:
    ; --- Cleanup ---
    mov rsp, rbp      ; Restore the stack pointer
    pop rbp           ; Restore the base pointer
    ret               ; Return the destination pointer (already in rax)
