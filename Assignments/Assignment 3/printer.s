;##########################     globals & externs   ########################## 
        global printer
        extern resume, system_call
        extern arrd2, columns

        
;##########################     macros & defines   ########################## 
sys_write:          equ     4
stdout:             equ     1
scheduler_idx:      equ     100*100

%macro sys_call 4                          ; system_call(arg1, arg2, arg3, arg4)
    push %4
    push %3
    push %2
    push %1
    call system_call
    add esp, 4*4
%endmacro

%macro print_digit 2                         
    cmp byte [arrd2+ecx], %1
    jne %%continue
    sys_call sys_write, stdout, %2, 1
    %%continue:
%endmacro
            
;##########################     section .rodata   ##########################
section .rodata

print_test:
	DB	        "test",10, 0
	
print_zero:
	DB	        "0", 0
	
print_one:
	DB	        "1", 0

print_two:
	DB	        "2", 0

print_three:
	DB	        "3", 0

print_four:
	DB	        "4", 0
	
print_five:
	DB	        "5", 0
	
print_six:
	DB	        "6", 0
	
print_seven:
	DB	        "7", 0
	
print_eight:
	DB	        "8", 0
	
print_nine:
	DB	        "9", 0
	
print_newline:
	DB	        10, 0

section .data

hello:  db 'hello', 10


;##########################     section .text    ##########################
section .text

;##########################     printer    ########################## 
printer:
        mov ecx, 0
        mov ebx, 0
        print_loop:
                cmp byte [arrd2+ecx], -1
                je  continue
                cmp ebx,dword[columns]
                je  new_row
                print_digit 0, print_zero
                print_digit 1, print_one
                print_digit 2, print_two
                print_digit 3, print_three
                print_digit 4, print_four
                print_digit 5, print_five
                print_digit 6, print_six
                print_digit 7, print_seven
                print_digit 8, print_eight
                print_digit 9, print_nine
                inc ecx
                inc ebx
                jmp print_loop
        
        new_row:
            mov ebx,0
            sys_call sys_write, stdout, print_newline, 1
            jmp print_loop
            
        continue:
            sys_call sys_write, stdout, print_newline, 1

        mov ebx, scheduler_idx
        call resume                                                 ; resume scheduler

        jmp printer