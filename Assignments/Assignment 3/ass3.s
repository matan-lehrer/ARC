;;; game of life
;;;
;;; Programmer: Eden Keshet & Ido Nitzan 2017

;##########################     globals & externs   ########################## 

global main, rows, columns, frequency, generations, arrd2, system_call
extern init_co, start_co, resume, scheduler, printer

;##########################     macros & defines   ########################## 
 
stdout:             equ     1
sys_exit:           equ     1
buffer_size:        equ     101*100
array_size:         equ     100*100
scheduler_idx:      equ     100*100
sys_open:           equ     5 
sys_read:           equ     3 
sys_write:          equ     4
O_RDONLY:           equ     0

           
%macro sys_call 4                          ; system_call(arg1, arg2, arg3, arg4)
    push %4
    push %3
    push %2
    push %1
    call system_call                             
%endmacro

%macro print_digit 2                         
    cmp byte [arrd2+ecx], %1
    jne %%continue
    sys_call sys_write, stdout, %2, 1
    %%continue:
%endmacro

%macro atoi 1
        pushad
        mov edx, 0              ;edx will holds the coverted number in the end of the loop
        %%conv_loop:
            cmp byte[ebx],0
            je %%cont
            mov ecx, 0              ;ecx holds the current char
            mov cl, byte[ebx]
            sub ecx,'0'
            imul edx, 10
            add edx, ecx
            inc ebx
            jmp %%conv_loop
        %%cont:
    
        mov dword [%1], edx
        popad
%endmacro
   
%macro check_str_length 1                       ; the length will be in ebx

     mov eax, dword [ecx + %1*4]                ; %1 is the index in rhe argv[]
        mov ebx, 0
        %%check_length:
            cmp byte [eax+ebx], 0               ; check if the sting ends
            je %%finish_check_length             ; if so finish loop
            inc ebx                             ; else increase length
            jmp %%check_length
        %%finish_check_length:
%endmacro

;##########################     section .rodata   ##########################
section .rodata

print_test:
	DB	        "test",10, 0
	
length:
	DB	        "length=", 0
	
width:
	DB	        "width=", 0
	
generations_num:
	DB	        "number of generations=", 0
	
frequency_num:
	DB	        "frequency=", 0
	
print_zero:
	DB	        "0", 0
	
print_one:
	DB	        "1", 0
	
print_newline:
	DB	        10, 0

;##########################     section .bss    ########################## 
section .bss
    buffer:             resb    buffer_size
    arrd2:              resb    array_size 
    
;##########################     section .data    ########################## 
section .data	
        rows:           dd 0
        columns:        dd 0
        frequency:      dd 0
        generations:    dd 0
        debug:          db 0
        argv:           dd 0

;##########################     section .text    ##########################
section .text
            
;##########################     Main    ########################## 
main:
        push ebp                            ; Get program arguments
        mov ebp, esp                        ;........  
        mov edx, dword [ebp+4+4*1]          ;........ edx <- argc
        cmp edx, 7                          ;........ if (argc==7) debug is on
        mov edx, 1                          ;........ if (debug) edx <- 2, else edx <- 1
        jne no_debug                        ;........
        mov byte [debug], 1                 ;........
        mov edx, 2                          ;........
        no_debug:                           ;........
        mov ecx, dword [ebp+4+4*2]          ;........   
        mov dword[argv], ecx                ;........ backup argv
        
        mov eax, dword [ecx + 4*edx]        ;........ eax = argv[1+debug]
        inc edx                             ;........
        
        mov ebx, dword [ecx + 4*edx]        ;........ ebx = argv[2+debug]
        inc edx                             ;........
        atoi rows                           ;........
        mov ebx, dword [rows]               ;........
        
        mov ebx, dword [ecx + 4*edx]        ;........ ebx = argv[3+debug]
        inc edx                             ;........
        atoi columns                        ;........
        
        mov ebx, dword [ecx + 4*edx]        ;........ ebx = argv[4+debug]
        inc edx                             ;........
        atoi generations
        mov ebx,dword[generations]          ;........ generations *= 2
        add ebx,ebx                         ;........
        mov dword[generations],ebx          ;........
        
        mov ebx,dword [ecx + 4*edx]         ;........ ebx = argv[5+debug]
        atoi frequency                      ;........
        ;mov ebx,dword[frequency]            ;........frequency *= 2
        ;add ebx,ebx                         ;........
        ;mov dword[frequency],3            ;......../
        
        sys_call sys_open, eax, O_RDONLY, 0 ; Open the file 
        
        sys_call sys_read, eax, buffer, buffer_size ; Read the file into the buffer

        mov ecx, 0                          ; Insert values into arrd2 array
        mov edx, 0                          ;........
        loop_val:                           ;........
            cmp byte [buffer+ecx], 0        ;........ 
            je continue                     ;........ 
            cmp byte [buffer+ecx], '1'      ;........
            je insert_one
            cmp byte [buffer+ecx], ' '      ;........
            je insert_zero                  ;........ 
            inc ecx                         ;........
            jmp loop_val                    ;........
            
            insert_one:                     ;........
                mov byte [arrd2+edx], 1     ;........
                inc ecx                     ;........
                inc edx                     ;........
                jmp loop_val                ;........
                
            insert_zero:                    ;........
                mov byte [arrd2+edx], 0     ;........
                inc ecx                     ;........
                inc edx                     ;........
                jmp loop_val                ;......../
            
        continue:
            mov byte [arrd2+edx], -1        ; Insert -1 at the end
            
        cmp byte [debug], 1                 ; If debug is on, make debug prints
        jne no_debug2                       ;........
        mov ecx, dword [argv]               ;......../ restore argv
        
        check_str_length 3                                              ; Debug Prints
        sys_call sys_write, stdout, length, 7                           ;........
        sys_call sys_write, stdout, dword [ecx + 4*3], ebx              ;........   
        sys_call sys_write, stdout, print_newline, 1                    ;........
        
        check_str_length 4                                              ;........
        sys_call sys_write, stdout, width, 6                            ;........
        sys_call sys_write, stdout, dword [ecx + 4*4], ebx              ;........
        sys_call sys_write, stdout, print_newline, 1                    ;........
        
        check_str_length 5                                              ;........
        sys_call sys_write, stdout, generations_num, 22                 ;........
        sys_call sys_write, stdout, dword [ecx + 4*5], ebx              ;........
        sys_call sys_write, stdout, print_newline, 1                    ;........
        
        check_str_length 6                                              ;........
        sys_call sys_write, stdout, frequency_num, 10                   ;........
        sys_call sys_write, stdout, dword [ecx + 4*6], ebx              ;........
        sys_call sys_write, stdout, print_newline, 1                    ;........
        
        mov ecx, 0                                                      ;........
        mov ebx, 0                                                      ;........
        print_loop:
                cmp byte [arrd2+ecx], -1                                ;........
                je  continue2                                           ;........
                cmp ebx,dword[columns]                                  ;........
                je  new_row                                             ;........
                print_digit 0, print_zero                               ;........
                print_digit 1, print_one                                ;........
                inc ecx                                                 ;........
                inc ebx                                                 ;........
                jmp print_loop                                          ;........
        
        new_row:                                                        ;........
            mov ebx,0                                                   ;........
            sys_call sys_write, stdout, print_newline, 1                ;........
            jmp print_loop                                              ;......../

        continue2:
        sys_call sys_write, stdout, print_newline, 1                    ; Print new line
        no_debug2:
        call init_co                                                    ; Initialize scheduler state
        
        mov ebx, scheduler_idx
        call start_co                                                   ; Start co-routines

        
        mov eax, sys_exit                                               ; exit
        mov ebx, 0                                                      ;........
        int 0x80                                                        ;......../
        
;##########################     system_call    ########################## 
system_call:
    push    ebp                                                         ; Save caller state
    mov     ebp, esp
    sub     esp, 4                                                      ; Leave space for local var on stack
    pushad                                                              ; Save some more caller state

    mov     eax, [ebp+8]                                                ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]                                               ; Next argument...
    mov     ecx, [ebp+16]                                               ; Next argument...
    mov     edx, [ebp+20]                                               ; Next argument...
    int     0x80                                                        ; Transfer control to operating system
    mov     [ebp-4], eax                                                ; Save returned value...
    popad                                                               ; Restore caller state (registers)
    mov     eax, [ebp-4]                                                ; place returned value where caller can see it
    add     esp, 4                                                      ; Restore caller state
    pop     ebp                                                         ; Restore caller state
    ret                                                                 ; Back to caller