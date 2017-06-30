;##########################     globals & externs   ########################## 
        global scheduler
        extern resume, end_co
        extern rows, columns, frequency, generations
        
;##########################     macros & defines   ########################## 
printer_idx:        equ     100*100 + 1
sys_exit:           equ     1
buffer_size:        equ     101*100
array_size:         equ     100*100
scheduler_idx:      equ     100*100
sys_open:           equ     5 
sys_read:           equ     3 
sys_write:          equ     4
O_RDONLY:           equ     0
     

;##########################     section .data    ########################## 
section .data	
        curr_generation:           db 0
        curr_frequency:           db 0
        i_backup:                dd 0

;##########################     section .text    ##########################
section .text

;##########################     scheduler    ########################## 
scheduler:
     .loop:  
        mov eax, 0                                  ; eax = i
        mov ecx, 0                                  ; ecx = j
        mov edx, 0                                  ; num of iterations
        
        mov dl, byte[frequency]                     ; Check if it's time to print.
        cmp dl, byte[curr_frequency]    
        je  resume_print1
    .cont1:
        mov dl, byte[generations]
        cmp dl, byte[curr_generation]
        je end_co

        .loop_rows:                                 ; Iterate over cells coroutines
            cmp al, byte[rows]
            je  .continue
            .loop_columns:
                mov [i_backup], eax                        ; backup i
                mul byte [columns]                  ; calculate index of co-routine
                add eax, ecx                        ; ......../
                mov ebx, eax                        ; ebx holds the index of the co-routine
                call resume
                inc byte[curr_frequency]            ; Check if it's time to to end
                mov dl, byte[frequency]                     ; Check if it's time to print.
                cmp dl, byte[curr_frequency]    
                je  resume_print2    
        .cont2:
            
                mov eax, [i_backup]                        ; restore i to eax
                inc ecx
                cmp cl, byte [columns]
                jne  .loop_columns
                mov ecx, 0                          ; ecx = j
                inc eax
                jmp .loop_rows
        .continue:
                inc byte[curr_generation]

                jmp .loop
  
;##########################     resume_print    ########################## 
resume_print1:
        mov byte[curr_frequency], 0
        mov ebx, printer_idx
        
        call resume
        jmp scheduler.cont1
    
    
    ;##########################     resume_print    ########################## 
resume_print2:
        mov byte[curr_frequency], 0
        mov ebx, printer_idx
        
        call resume
        jmp scheduler.cont2