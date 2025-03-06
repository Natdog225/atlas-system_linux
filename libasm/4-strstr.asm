; strstr
; @rdi: haystack string
; @rsi: needle string
;
; return: rax with location of beginning of str match, or null if not found

section .text
	global asm_strstr

asm_strstr:
	; The main function implementing the strstr functionality.

.outerLoop:
	; Load a byte from the haystack into al.
	mov al, byte [rdi]
	; Load a byte from the needle into bl.
	mov bl, byte [rsi]

	; Check if the needle is empty. If so, return the haystack.
	cmp bl, 0
	je .found

	; Compare the current haystack byte with the first needle byte.
	cmp al, bl
	je .matchingFirstChar  ; If they match, proceed to check the rest.

	; Check for the end of the haystack.
	cmp al, 0
	je .notFound       ; If end of haystack, needle not found.

	; Move to the next character in the haystack.
	inc rdi
	jmp .outerLoop      ; Continue the outer loop.


.matchingFirstChar:
	; Save the current haystack position (potential start of match).
	mov rbx, rdi
	; Save the needle's starting position.
	mov rcx, rsi

.innerLoop:
	; Advance pointers for both haystack and needle.
	inc rbx
	inc rcx

	; Load the next byte from the haystack (using the saved position).
	mov al, byte [rbx]
	; Load the next byte from the needle.
	mov dl, byte [rcx]

	; Check if we've reached the end of the needle (full match).
	cmp dl, 0
	je .found

	; Check if we've reached the end of the haystack.
	cmp al, 0
	je .notFound

	; Compare the current bytes from haystack and needle.
	cmp al, dl
	je .innerLoop  ; If they match, continue the inner loop.

	; If bytes don't match, increment the original haystack pointer
	; and restart the outer loop.
	inc rdi
	jmp .outerLoop

.notFound:
	; Needle not found, return NULL (0).
	xor rax, rax
	ret

.found:
	; Needle found, return the starting address of the match (rdi).
	mov rax, rdi
	ret
