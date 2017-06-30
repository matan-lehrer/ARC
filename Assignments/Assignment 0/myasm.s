section .data                    	; data section, read-write
        an:    DD 0              	; this is a temporary var

section .text                    	; our code is always in the .text section
        global do_Str          	; makes the function appear in global scope
        extern printf            	; tell linker that printf is defined elsewhere 							; (not used in the program)

do_Str:                        	; functions are defined as labels
        push    ebp              	; save Base Pointer (bp) original value
        mov     ebp, esp         	; use base pointer to access stack contents
        pushad                   	; push all variables onto stack
        mov ecx, dword [ebp+8]	; get function argument

;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE STARTS HERE ;;;;;;;;;;;;;;;; 

	mov	dword [an], 0		; initialize answer
	label_here:
                    ; check if char equals to '('
                    cmp byte [ecx], '('
                    ; if holds change char to '<'
                    je equals_left_bracket
                    ; check if char equals to ')'
                    cmp byte [ecx], ')'
                    ; if holds change char to '>'
                    je equals_right_bracket
                    ; check if char greater than 'a'
                    cmp byte [ecx], 'a'
                    ; If holds, go to appropriate block
                    jge greater_than_a
                    ; else check if char greater than 'A'
                    cmp byte [ecx], 'A'
                    ; If holds, go to appropriate block
                    jge greater_than_A
                    ; Else, less than A, increase counter
                    inc dword [an]
                    ; finish
                    jmp after_block
        greater_than_a:  
                    cmp byte [ecx], 'z'
                    ; If holds, go to appropriate block
                    jle less_than_z
                    ; Else, increase counter
                    inc dword [an]
                    ; finish
                    jmp after_block
        greater_than_A:  
                    cmp byte [ecx], 'Z'
                    ; If holds, go to appropriate block
                    jle after_block
                    ; Else between Z and a, increase counter
                    inc dword [an]
                    ; finish
                    jmp after_block
        less_than_z:  
                    ; make upper case
                    sub byte [ecx], 32
                    ; finish
                    jmp after_block
        equals_left_bracket:
                    ; replace '(' with '<'
                    add byte [ecx], 20
                    ; increase counter
                    inc dword [an]
                    ; finish
                    jmp after_block
        equals_right_bracket:
                    ; replace ')' with '>'
                    add byte [ecx], 21
                    ; increase counter
                    inc dword [an]
                    ; finish
                    jmp after_block
        
        
        after_block: ; Finish block of code
                    

		inc ecx      	    ; increment pointer
		cmp byte [ecx], 0   ; check if byte pointed to is zero
		jnz label_here      ; keep looping until it is null terminated


;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE ENDS HERE ;;;;;;;;;;;;;;;; 

         popad                    ; restore all previously used registers
         mov     eax,[an]         ; return an (returned values are in eax)
         mov     esp, ebp
         pop     ebp
         ret 
		 