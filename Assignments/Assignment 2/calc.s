;;; calc.s
;;;
;;; Programmers: Eden Keshet & Ido Nitzan 2017

                ;##########################     section .data   ########################## 
section .data	
fiveStack:              dd 0,0,0,0,0
stackPointer:           db 0
counter:                db 0
link_address:           dd 0
operations_counter:     db 0
carry_flag:             db 0
shift_counter:          db 0
debug_bool:             db 0

                ;##########################     section .rodata   ########################## 
section	.rodata
format_string:
	DB	        "%s", 0	; Format string
format_number:
	DB	        "%d", 0	; Format string
format_char:
	DB	        "%c", 0	; Format string
sof_msg:
	DB	        ">>Error: Operand Stack Overflow", 10, 0
ii_msg:
	DB	        ">>Error: Illegal Input", 10, 0
in_msg:
	DB	        ">>Error: Insufficient Number of Arguments on Stack", 10, 0
ex_msg:
        DB	         ">>Error: exponent too large", 10, 0
new_cmd:
	DB	        ">>calc: ", 0
print_output:
	DB	        ">>", 0
test_here:
	DB	        "test", 10, 0
print_debug:
	DB	        "debug: ", 0
print_new_line:
	DB	        10, 0
	

            ;##########################     section .bss    ########################## 
section .bss
    str:	RESB	1024
    
            ;##########################     macros   ########################## 

%macro print 2                          ; Print(*str, format)
    pushad                              ; backup registers
    pushfd                              ; backup flags register

    push    %1		                ; Call printf with 2 arguments: pointer to str
    push    %2		                ; and pointer to format 
    call    printf                      ; call int printf(char *format, arg list …)
    add esp, 4*2                        ; Clean up stack after call
    
    popfd                               ; restore flags register
    popad                               ; restore registers
%endmacro

%macro print_err 2
    pushad                              ; backup registers
    pushfd                              ; backup flags register

    push    %1		                ; Call printf with 2 arguments: pointer to str
    push    %2		                ; and pointer to format string.
    push    dword [stderr]
    call    fprintf                      ; call int printf(char *format, arg list …)
    add esp, 4*3                        ; Clean up stack after call
    
    popfd                               ; restore flags register
    popad                               ; restore registers
%endmacro

%macro check_first_char 2		; Check the value of the first char
    cmp byte [str], %1			;........
    je %2				;......../
%endmacro

%macro check_error_invalid_input 0      ; Check if this is the end of the string
    cmp byte [str+1], 10                ;........
    jne error_invalid_input             ;......../
%endmacro

%macro check_error_insufficient 1       ; Check if there is an element in the stack
    cmp byte [stackPointer], %1         ;........
    je error_insufficient               ;......../
 
%endmacro

%macro check_error_stack_over_flow 1    ; Check if the stack is full
    cmp byte [stackPointer], %1         ;........
    je error_stack_over_flow            ;......../
        
%endmacro

%macro check_error_invalid_number 0     ; Check if the string is a valid number. string srt = holds the input user, int ecx = holds the index in the loop
    mov ecx, 0                          ;.......
    %%loop_val:                         ;.......
        cmp byte [str+ecx], 10          ;....... 
        je %%continue                   ;........ check if it's the end of the string
        cmp byte [str+ecx], '0'         ;........
        jl error_invalid_input          ;........ check if it's lower than zero 
        cmp byte [str+ecx], '9'         ;........
        jg error_invalid_input          ;........ check if it's higher than nine
        inc ecx                         ;........
        jmp %%loop_val                  ;......../
    %%continue:  
%endmacro
 
%macro add_link 0                       ; Adding a link to the list
    pushad                              ;........ backup registers
    pushfd                              ;........ backup flags register

    push	5		        ;........ call malloc
    call	malloc                  ;........
    mov dword [link_address],   eax     ;........
    add 	esp, 4	                ;........ Clean up stack after call
                
    popfd                               ;........
    popad                               ;........

    mov     eax, dword [link_address]   ;........
                
    pushad                              ;........
    pushfd                              ;........

    mov     byte [eax], dl              ;........ *eax  <- dl. move the number to the address pointed by eax
    mov     ebx, 0                      ;........
    mov     bl, byte [stackPointer]     ;........ bl <- &stackPointer. bl is now the top of the stack
    mov     ecx, dword [fiveStack+ebx*4];........ ecx <- &[fiveStack + stackPointer]. ecx is now the address of the last link
    mov     dword [eax + 1], ecx        ;........ connect the current link with the next link
    mov     dword [fiveStack+ebx*4], eax;........ insert a new link
                
    popfd                               ;........
    popad                               ;......../
%endmacro
 
%macro duplicate_ret 1                  ; Duplicate a number. 
    call pop_stack                      ;........ pop, the returned value is in eax (the address of the popped link)
    
    mov ebx, 0                          ;........
    mov bl, byte [stackPointer]         ;........ bl <- &stackPointer. bl is now the top of the stack
    mov dword [fiveStack + ebx*4], eax  ;........ ecx <- &[fiveStack + stackPointer]. ecx is now the address of the last link
    inc byte [stackPointer]             ;........ 

    push 'z'                            ;........ push null('z') to stack in order to know when to stop duplicating
    %%loop:                             ;........
        mov ebx, 0                      ;........
        mov bl, byte [eax]              ;........ ebx holds the BCD number.
        push ebx                        ;........ backup ebx in the stack

        mov eax, dword [eax+1]          ;........ move to next link
        cmp eax, 0                      ;........ check if this is the end of the linked list
        
        jne %%loop                      ;........ else continue printing
        %%duplicate_popped:             ;........
            pop edx                     ;........ edx holds the char to print
            cmp edx, 'z'                ;........ if this is null('z') finish printing
            je %1                       ;........ the return value is in the first parm
            
            add_link                    ;........
            
            jmp %%duplicate_popped      ;......../ 
%endmacro

%macro addition_ret 1                   ; Adding two numbers
    call pop_stack                      ;........ call pop, the returned value is in eax (the address of the popped link)
    
    mov ecx, eax                        ;........
    call pop_stack                      ;........ call pop, the returned value is in eax (the address of the popped link)
    mov ebx, eax                        ;........
    push 'z'                            ;........ push 'z' to stack in order to know when to stop printing
    mov byte [carry_flag], 0            ;........ saving the cf in carry_flag

    %%loop:                             ;........
        mov eax, 0                      ;........ eax will hold the first number.  
        mov edx, 0                      ;........ edx will hold the second number.
        mov al, byte [ebx]              ;........ eax holds the hex number.
        mov dl, byte [ecx]              ;........ edx holds the hex number.
        add al, byte[carry_flag]        ;........ al <- al + cf
        add al, dl                      ;........ al <- al + dl
        daa                             ;........ convert from hexdecimel to bcd
        
        mov byte [carry_flag], 1        ;........ *carry_flag <- 1.
        jc %%continue1                  ;........
            mov byte [carry_flag], 0    ;........ *carry_flag <- 0.
        %%continue1:                    ;........
        push eax                        ;........ backup eax in the stack
       
        mov ebx, dword [ebx+1]          ;........ move to next link
        cmp ebx, 0                      ;........ check if this is the end of the linked list
        je %%first_operand_terminated   ;........
        mov ecx, dword [ecx+1]          ;........ move to next link
        cmp ecx, 0                      ;........ check if this is the end of the linked list
        je %%second_operand_terminated  ;........

        jmp %%loop                      ;........ else continue printing
        
        %%first_operand_terminated:     ;........
            mov ecx, dword [ecx+1]      ;........ move to next link
            cmp ecx, 0                  ;........            
            je %%add_carry              ;........
            mov eax, 0                  ;........ eax will hold the number. eax <- 0
            mov al, byte [ecx]          ;........ eax holds the hex number.
            add al, byte[carry_flag]    ;........ al <- al + cf
            daa                         ;........ convert from hexdecimel to bcd              
            
            mov byte [carry_flag], 1    ;........ *carry_flag <- 1.
            jc %%continue2              ;........
            mov byte [carry_flag], 0    ;........ *carry_flag <- 0.
            %%continue2:                ;........     
            push eax                    ;........ backup eax in the stack
            
            jmp %%first_operand_terminated  ;........
            
        %%second_operand_terminated:    ;........
            mov eax, 0                  ;........ eax will hold the number. eax <- 0
            mov al, byte [ebx]          ;........ ebx holds the hex number.
            add al, byte[carry_flag]    ;........ al <- al + cf
            daa                         ;........ convert from hexdecimel to bcd
            
            mov byte [carry_flag], 1    ;........ *carry_flag <- 1.
            jc %%continue3              ;........ 
            mov byte [carry_flag], 0    ;........ *carry_flag <- 0.
            %%continue3:                ;........       
            push eax                    ;........ backup eax in the stack
                      
            mov ebx, dword [ebx+1]      ;........ move to next link
            cmp ebx, 0                  ;........
            je %%add_carry              ;........
            jmp %%second_operand_terminated ;........

        %%add_carry:                    ;........
        cmp byte[carry_flag], 1         ;........
        jne %%insert_new_link           ;........
            push 1                      ;........
        %%insert_new_link:              ;........
            pop edx                     ;........ edx holds the char to print
            cmp edx, 'z'                ;........ if this is  null( 'z') finish printing
            je %1                       ;........ 
            
            add_link                    ;........
            
            jmp %%insert_new_link       ;......../
%endmacro

            ;##########################     section .text   ########################## 
section .text
     extern printf 
     extern fprintf 
     extern malloc 
     extern free
     extern fgets 
     extern stderr 
     extern stdin 
     extern stdout 
     extern exit
     
align 16 
global main

            ;##########################     Main    ########################## 
main:
    push ebp                            ; Chack for debug flag in args
    mov ebp, esp                        ;........      
    mov ebx, dword [ebp+4+4*2]          ;........
    cmp dword [ebx+4], 0                ;........
    je jump_my_calc                     ;........
    mov ecx, dword [ebx+4]              ;........
    cmp byte [ecx], '-'                 ;........
    jne jump_my_calc                    ;........
    cmp byte [ecx+1], 'd'               ;........
    jne jump_my_calc                    ;........
    mov byte [debug_bool], 1            ;......../
    
jump_my_calc:
    call my_calc

    mov eax, 0                          ; Print the number of operations
    mov al, byte [operations_counter]   ;........
    print eax, format_number            ;........
    print   print_new_line, format_string;......../

    mov eax, 0                          ; exit program
    call exit

            ;##########################     My calc    ########################## 
my_calc:
    print new_cmd, format_string
    
    call get_input
        
    check_first_char 'q', maybe_quit
    check_first_char '+', maybe_add
    check_first_char 'p', maybe_pop_print
    check_first_char 'd', maybe_duplicate
    check_first_char 'r', maybe_shift_right
    check_first_char 'l', maybe_shift_left
    
    jmp maybe_push_number
        
    maybe_quit:                         ;*************      maybe quit      *************      
        check_error_invalid_input
        jmp quit                 
        
    maybe_add:                          ;*************      maybe add      *************  
        check_error_invalid_input
        check_error_insufficient 0
        check_error_insufficient 1
        inc byte [operations_counter]
        jmp addition
        
    maybe_pop_print:                    ;*************      maybe pop_print      *************  
        check_error_invalid_input
        check_error_insufficient 0
        inc byte [operations_counter]
        jmp pop_and_print
        
    maybe_duplicate:                    ;*************      maybe duplicate      *************  
        check_error_invalid_input
        check_error_insufficient 0
        check_error_stack_over_flow 5
        inc byte [operations_counter]
        jmp duplicate
        
    maybe_shift_right:                  ;*************      maybe shift_right      *************  
        check_error_invalid_input
        check_error_insufficient 0
        check_error_insufficient 1
        jmp shift_right
        
    maybe_shift_left:                   ;*************      maybe shift_left      *************  
        check_error_invalid_input
        check_error_insufficient 0
        check_error_insufficient 1
        jmp shift_left
        
    maybe_push_number:                  ;*************      maybe push_number      *************  
        check_error_invalid_number
        check_error_stack_over_flow 5
        cmp byte[debug_bool], 0
        je push_number
        print_err print_debug, format_string
        print_err str, format_string
        jmp push_number
        
            ;##########################     Quit    ##########################        
quit:
    ret
            ;##########################     Addition    ##########################        
addition:
    addition_ret conv_end
    
            ;##########################     Pop_and_print    ##########################        
pop_and_print:
     call pop_stack                     ; call pop, the returned value is in eax (the address of the popped link)
    push 0                              ; push null to stack in order to know when to stop printing
        
    .loop:
        mov ebx, 0                      ; ebx will hold the number. ebx <- 0
        mov bl, byte [eax]              ; ebx holds the BCD number.
        mov ecx, ebx
        
        and ecx, 15                     ; ecx <- ecx modulo 16
        add ecx, '0'                    ; convert to char
        push ecx
            
        and ebx, 240                    ; ecx <- ecx and 11110000(b)
        shr ebx, 4                      ;ecx <- ecx/16
        add ebx, '0'                    ; convert to char
        push ebx

        mov eax, dword [eax+1]          ; move to next link
        cmp eax, 0                      ; check if this is the end of the linked list
        jne .loop                       ; else continue printing
            
    print print_output, format_string
            
    pop edx
    cmp edx, '0'
    je .print_popped
    print edx, format_char
            
    .print_popped:
        pop edx                         ; edx holds the char to print
        cmp edx, 0                      ; if this is null finish printing
        je .print_new_line              ; ........./
        print edx, format_char          ; print the char
        jmp .print_popped               ; continue printing
                
    .print_new_line:
        print print_new_line, format_string
        jmp my_calc
        
            ;##########################     Duplicate    ##########################        
duplicate:
    duplicate_ret conv_end        

            ;##########################     Shift right    ##########################        
shift_right:
    call pop_stack                          ; get shift times to eax
    mov ebx, dword [eax+1]                  ; ebx holds address of the next link
    cmp ebx, 0                              ; if there is next link, the exponent is too large
    jne error_exponent_too_large            ;......../
    mov ecx, 0                              ;
    mov cl, byte [eax]                      ; ecx holds the shift counter.
    mov byte [shift_counter], cl            ; backup shift counter
     
    inc byte [operations_counter]
    
    shl_loop:
        call pop_stack
        mov edx, eax                        ; edx holds the pointer to the current link
        push eax                            ; backup the orignal link address's
        div_by_two_loop:
            mov ebx, 0                      ; ebx will hold the decimal value of the link
            mov eax, 0                      ; eax is a temporal for the conversion
            mov bl, byte [edx]              ; convert bl from bcd to decimal
            mov al, byte [edx]              ;..........
            and bl, 15                      ;..........
            and al, 240                     ;..........
            shr al, 4                       ;..........
            mov cl, 10                      ;..........
            mul cl                          ;..........
            add bl, al                      ;........../
            shr bl, 1                       ; bl <- bl/2
            mov eax, dword [edx+1]          ; check if the next link's value is odd, if so add the remainder (50)
            cmp eax, 0
            je  even_next
            mov cl, byte [eax]              ;.......
            and cl, 1                       ;.......
            cmp cl, 0                       ;.......
            je even_next
                add bl, 50                  ;......./
                
            even_next:
            mov eax, 0
            mov al, bl                      ; add remainder if needed
            mov bl, 10                      ; convert from decimal to bcd in order to insert to the link
            div bl                          ;........
            mov ebx, 0                      ;........
            mov bl, ah                      ;........
            shl al, 4                       ;........
            add bl, al                      ;......../
            mov byte [edx], bl              ; insert new value to link
            
            mov ebx, dword [edx+1]          ; check if the next link is the last one and if its value is 1, if so ignore this link
            cmp ebx, 0                      ;......
            je cont                         ;......
            mov ecx, dword [ebx+1]          ;......
            cmp ecx, 0                      ;......
            jne cont                        ;......
            cmp byte [ebx], 1               ;......
            jne cont                        ;......
            mov dword [edx+1], 0            ;....../
            jmp insert_link                 ;
            
            cont:
            cmp dword [edx+1], 0            ; check if this is the end of the linked list
            je insert_link                  ; if so insert the modified linked list
            mov edx, dword [edx+1]          ; else, move to the next link
            jmp div_by_two_loop
            insert_link:
                pop eax
                mov     ebx, 0                              ;bl is the top of the stack
                mov     bl, byte [stackPointer]             ;............../
                mov     dword [fiveStack+ebx*4], eax        ;insert a new link
                inc     byte [stackPointer]
                
                mov al, byte [shift_counter]                ; update shift couneter    
                sub al, 1                                   ;.......
                das                                         ;.......
                mov byte [shift_counter], al                ;......./
                cmp al, 0                                   ; check if should be done another division
                jne shl_loop
                jmp my_calc
    
    
            ;##########################     Shift left    ##########################        
shift_left:
    call pop_stack                          ; get shift times to eax
    mov ebx, dword [eax+1]
    cmp ebx, 0
    jne error_exponent_too_large
    mov ecx, 0
    mov cl, byte [eax]                      ; eax holds the BCD number.
    mov byte [shift_counter], cl
    
    inc byte [operations_counter]
    
    loop:
        duplicate_ret shift_left_dup_ret 
        
        shift_left_dup_ret:
            inc byte [stackPointer]         ; increase stack pointer

        addition_ret shift_left_add_ret

        shift_left_add_ret:
            inc byte [stackPointer]         ; increase stack pointer

        mov eax, 0
        mov al, byte [shift_counter]
        sub al, 1
        das
        mov byte [shift_counter], al
        cmp byte [shift_counter], 0
        jne loop
    jmp my_calc
        
            ;##########################     Push number    ##########################        
push_number:
        mov ecx, 0                          ; ecx will hold the string length
        cmp byte [str+ecx], 10              ; check if the sting ends
        je my_calc
        
        cmp byte [str+ecx], '0'             ; check for 0 in the begging og the str.        
        jne .backup
        
       .check_char0:
            inc ecx                         ; else increase length
            cmp byte [str+ecx], '0'         ; check if the sting ends
            je .check_char0                 ; if so go to loop2
            cmp byte [str+ecx], 10          ; check if the sting ends
            jne .backup                     ; if so go to loop2
            dec ecx
        
        .backup:        
            mov edx, ecx                    ;backup ecx
        .check_length:
            cmp byte [str+ecx], 10          ; check if the sting ends
            je .finish_check_length         ; if so finish loop
            inc ecx                         ; else increase length
            jmp .check_length
        .finish_check_length:
            sub ecx, edx
            and ecx, 1                      ; check if the length is odd or even
            cmp ecx, 0                      ;............../
            
            mov ecx, edx                    ; ecx is the counter to the number of chars we read from the user
            je .loop                        ; if even go to loop, else take care of first digit
            
            mov edx, 0                      ; edx holds two chars; dh contains the first and dl contains the second
            mov dl, byte [str + ecx]        ; move first digit to dh
            sub dl, 48                      ; convert to number
                    
            jmp .insert_new_link

        .loop:
            mov edx, 0                      ; edx holds two chars; dh contains the first and dl contains the second
            mov dh, byte [str+ecx]          ; move first digit to dh
            cmp dh, 10                      ; end of user input
            je conv_end                     ;............;
            sub dh, 48                      ; convert to number
            
            inc ecx                         ; move to next char
            mov dl, byte [str+ecx]          ; move second digit to dl
            mov bl, byte [str+ecx]
            shl dh, 4                       ; dh <- dh*16
            sub dl, 48                      ; convert to number
            add dh, dl                      ; dh <- dh+dl
            mov dl, dh                      ; dl <- dh
            mov         dh,0                ; reset dh
         
            .insert_new_link:
                add_link
                inc ecx                     ; ecx is the counter to the number of chars we read from the user
                jmp .loop                   ; continue with loop

conv_end:                   ;*************      conv end      ************* 
    inc byte [stackPointer]                 ; increase stack pointer
    jmp my_calc                             ; return to my_calc
                
get_input:                  ;*************      get input      ************* 
    pushad                                  ; backup registers
    pushfd                                  ; backup flags register
        
    push dword [stdin]                      ; push argument to fgets
    push 1024                               ; another one...
    push str                                ; another one...
    call fgets                              ; call  fgets(buffer, BUFFERSIZE , stdin)
    add esp, 4*3                            ; Clean up stack after call

    popfd                                   ; restore flags register
    popad                                   ; restore registers        
    ret

pop_stack:                  ;*************      pop tack      *************
    mov     ebx, 0                          ;bl is the top of the stack
    dec byte [stackPointer]                 ; decrease stack pointer 

    mov     bl, byte [stackPointer]         ;............../
    mov     eax, dword [fiveStack+ebx*4]    ;eax is the address of the last link

    mov dword [fiveStack+ebx*4], 0          ; insert null to current top of stack
    ret                                     ; return value is in eax (the address of the popped link)

error_stack_over_flow:      ;*************      error stack over flow      *************
    print sof_msg, format_string
    jmp my_calc

error_invalid_input:        ;*************      error invalid input      *************
    print ii_msg, format_string
    jmp my_calc

error_insufficient:         ;*************      error insufficient      *************
    print in_msg, format_string
    jmp my_calc

error_exponent_too_large:   ;*************      error exponent too large      *************
    mov     ebx, 0                          ;return the old link into the stack
    mov     bl, byte [stackPointer]         ;........ bl <- &stackPointer. bl is now the top of the stack
    mov     dword [fiveStack+ebx*4], eax    ;......../ insert a new link
    inc     byte [stackPointer]
    
    print ex_msg, format_string
    jmp my_calc
