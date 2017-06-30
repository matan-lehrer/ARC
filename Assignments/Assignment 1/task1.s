section	.rodata
LC0:
	DB	"%s", 10, 0	; Format string

section .bss
LC1:
	RESB	256

section .text
	align 16
	global my_func
	extern printf

my_func:
	push	ebp
	mov	ebp, esp	; Entry code - set up ebp and esp
	pushad			; Save registers

	mov ecx, dword [ebp+8]	; Get argument (pointer to string)
	mov edx, 0
        label_here:
                cmp byte [ecx], 10      ; check if input is empty
                je func_end             ; if so, jump to the end
                mov bl, 16              ; multiplier
                mov al, byte [ecx]      ; multiplicand
                sub al, 48
                mul bl                  ; result of multiplication is in ax
                inc ecx                 ; increment pointer
                cmp byte [ecx], '9'     ; check if the char is digit or letter
                jle char_is_digit       ; if it is digit go to appropriate block
                cmp byte [ecx], 'F'     ; check if the char is is upper case
                jle char_is_upper_case  ; if it is upper case go to appropriate block
                jmp char_is_lower_case  ; else, char is lower case
        char_is_digit:
                add al, byte [ecx]
                sub al, 48
                jmp iter_end
        char_is_upper_case:
                add al, byte [ecx]
                sub al, 55
                jmp iter_end
        char_is_lower_case:
                add al, byte [ecx]
                sub al, 87
                jmp iter_end
        iter_end:                       ; Finish iteration
                mov byte [LC1+edx], al
                inc edx
		inc ecx      	        ; increment pointer
		cmp byte [ecx], 10      ; check if byte pointed to is zero
		jnz label_here          ; keep looping until it is null terminated
        func_end:

	push	LC1		; Call printf with 2 arguments: pointer to str
	push	LC0		; and pointer to format string.
	call	printf
	add 	esp, 8		; Clean up stack after call

	popad			; Restore registers
	mov	esp, ebp	; Function exit code
	pop	ebp
	ret