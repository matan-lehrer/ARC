section	.rodata
LC0:
	DB	"%s", 10, 0	; Format string
LC1:
	DB	"x or k, or both are off range", 0
LC2:
	DB	"%d", 10, 0	; Format string

section	.data
	answer:    DD 0

section .text
	global calc_div
	extern printf
	extern check

calc_div:
	push	ebp
	mov	ebp, esp       ; Entry code - set up ebp and esp
	sub esp, 4             ; allocate space for local var
        mov ecx, [ebp+12]      ; Get second argument (k)
	mov ebx, [ebp+8]       ; Get first argument (x)
	mov [ebp-4], ebx       ; move x to the stack
	pushad                 ; backup registers
	pushfd                 ; backup eflags
	push ecx               ; Call check(x, k)
	push ebx
	call check
	mov [answer], eax      ; move answer to eax
	add esp, 8             ; Clean up stack after call
	popfd                  ; Restore eflags
	popad                  ; Restore registers
	cmp dword [answer], 0  ; if x or k are illegal jump to illegal_input
	je illegal_input
	mov ebx, 1             ; ebx <- 1
	shl ebx, cl            ; ebx <- 2^k
	mov eax, [ebp-4]       ; eax <- x
	div ebx                ; eax <- eax/ebx
	pushad                 ; backup registers
        pushfd                 ; backup eflags
        push	eax	       ; Call printf with 2 arguments: int (z)
        push	LC2	       ; and pointer to format number
        call	printf
        add 	esp, 8	       ; Clean up stack after call
        popfd                  ; restore eflags
        popad		       ; Restore registers
        jmp end_func
	illegal_input:
                pushad                  ; backup registers
                pushfd                  ; backup eflags
                push	LC1		; Call printf with 2 arguments: pointer to str
                push	LC0		; and pointer to format string.
                call	printf
                add 	esp, 8		; Clean up stack after call
                popfd                   ; Restore eflags
                popad			; Restore registers
	end_func:
                mov	esp, ebp	; Function exit code
                pop	ebp
                ret