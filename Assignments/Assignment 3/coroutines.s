;##########################     globals & externs   ########################## 
        global init_co, start_co, end_co, resume 
        global maxcors
        extern rows, columns, frequency, generations, arrd2
        extern printer, scheduler, cell

;##########################     macros & defines   ########################## 
maxcors:                equ 100*100+2           ; maximum number of co-routines
stacksz:                equ 16*1024             ; per-co-routine stack size
scheduler_idx:          equ 100*100
printer_idx:            equ 100*100 + 1

;##########################     section .bss    ########################## 
section .bss

stacks: resb maxcors * stacksz  ; co-routine stacks
cors:   resd maxcors            ; simply an array with co-routine stack tops
curr:   resd 1                  ; current co-routine
origsp: resd 1                  ; original stack top
tmp:    resd 1                  ; temporary value


;##########################     section .text    ##########################
section .text
;##########################     init_co    ########################## 
init_co:                                        ; Init all coroutines
        mov edx, 0                              ; edx = j
        mov ebx, 0                              ; ebx = i
        .loop_rows: 
            cmp bl, byte[rows]
            je  .continue
            .loop_columns:
                call init_co_cell               ; init cell(ebx, edx) coroutine
                inc edx
                cmp dl, byte [columns]
                jne  .loop_columns
                mov edx, 0       
                inc ebx
                jmp .loop_rows
        .continue: 
        jmp init_co_scheduler                   ; init scheduler coroutine
        .continue2:
        jmp init_co_printer                     ; init printer coroutine
        .continue3:
        ret                                     ; return to caller
        
        
;##########################     init_co_scheduler    ########################## 
init_co_scheduler:                                  ; Init scheduler
        pushad
        pushfd
                                                    ; calculate the address of the current coroutine stack top
        mov eax,    stacksz                         ; eax <- 16*1024
        mov ebx, scheduler_idx                      ; ebx <- 100*100
        mul ebx                                     ; eax <- 100*100    *  16*1024
        mov ecx, eax                                ; ecx <- 100*100    *  16*1024
        add ecx, stacks                             ; ecx <- stacks     +  100*100  *  16*1024
        add ecx, stacksz                            ; ecx <- stacksz    +  stacks   +  100*100  *  16*1024        
        mov dword [origsp], esp
        mov esp, ecx
        push scheduler                              ; push scheduler func address
        pushfd                                      ; backup coroutines flags
        pushad                                      ; backup coroutines regs
        mov dword [cors + scheduler_idx*4], esp
        mov esp, dword [origsp]
        
        popfd
        popad
        jmp init_co.continue2
                
;##########################     init_co_printer    ########################## 
init_co_printer:                                    ; Init printer
        pushad
        pushfd
                                                    ; calculate the address of the current coroutine stack top
        mov eax,    stacksz                         ; eax <- 16*1024
        mov ebx, printer_idx                        ; ebx <- 100*100 + 1
        mul ebx                                     ; eax <- (100*100+1)    *  16*1024
        mov ecx, eax                                ; ecx <- (100*100+1)   *  16*1024
        add ecx, stacks                             ; ecx <- stacks     +  (100*100+1)  *  16*1024
        add ecx, stacksz                            ; ecx <- stacksz    +  stacks   +  (100*100+1)  *  16*1024        
        mov dword [origsp], esp
        mov esp, ecx
        push printer                                ; push printer func address
        pushfd                                      ; backup coroutines flags
        pushad                                      ; backup coroutines regs
        mov dword [cors + printer_idx*4], esp
        mov esp, dword [origsp]
        
        
        popfd
        popad
        jmp init_co.continue3

    
;##########################     init_co_cell    ########################## 
init_co_cell:                                       ; Init cell coroutine(ebx = i, edx = j)
        pushad
        pushfd
        
        mov eax,    0                               ; calculate the address of the current coroutine stack top
        mov al,     byte [columns]
                                                    
        mul bl                                      ; eax = columns*i
        add eax,    edx                             ; eax = columns*i + j
        mov ebx,    eax                             ; backup eax
        mov ecx, stacksz
        mul ecx                                     
        add ecx, eax
        add ecx, stacks
        mov eax, ebx                                ; restore eax
        mov dword [origsp], esp                     ; backup original esp       
        mov esp, ecx                                ; move to the stack of the cor
        push cell_func                              ; push cell_func address
        pushfd                                      ; backup coroutines flags
        pushad                                      ; backup coroutines regs
        mov dword [cors + eax*4], esp
        mov esp, dword [origsp]
        popfd
        popad
        ret

;##########################     cell_func    ########################## 
cell_func:                                          ; manage cell coroutine
        mov eax, ebx                                ; eax = c*i +j
        mov ecx, 0
        mov cl, byte[columns]
        mov edx, 0
        div ecx                                     ; eax = i, edx = j

        push edx                                    ; call cell with the correct parameters
        push eax
        call cell                                   ; eax gets the return value from cell
        add esp, 2*4
        mov ebx, scheduler_idx  
        call resume                                 ; resume scheduler
        
        mov byte [arrd2+ebx], al                    ; update arrd2 with the returned value
        
        mov ebx, scheduler_idx  
        call resume                                 ; resume scheduler
        
        jmp cell_func

;##########################     start_co    ########################## 
start_co:                                           ; start the program
        pushfd
        pushad                                      ; save all registers (restored in "end_co")
        mov [origsp], esp                           ; save caller's stack top
        mov [curr], ebx                             ; store current co-routine index
        jmp do_resume                               ; perform state-restoring part of "resume"

;##########################     end_co    ########################## 
end_co:                                             ; end the program
        mov esp, [origsp]                           ; restore stack top of whoever called "start_co"
        popad                                       ; restore all registers
        popfd   
        ret                                         ; return to caller of "start_co"

        ;; ebx = co-routine index to switch to
;##########################     resume    ########################## 
resume:                                             ; resume the coroutine that in ebx
        pushfd                                      ; save flags to source co-routine stack
        pushad                                      ; save all registers
        
        mov edx, dword [curr]                       ; save previous co-routine index in edx
        mov dword [cors + edx*4],esp                ; save stack pointer of the previous co-routine

;##########################     do_resume    ########################## 
do_resume:
        mov esp, dword [cors + ebx*4]               ; move the esp to be the stack pointer of the new co-routine                
        mov dword [curr], ebx                       ; move curr to be the new co-tourine index
        
        popad
        popfd
        mov ebx, dword [curr]
        ret