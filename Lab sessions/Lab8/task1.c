#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <elf.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>


#define name_size 100

#define menu_size 7

#define Failed_Exit -1

#define Success_Exit 0

struct menuItem {
    char *action;
    void (*fun)();
};

void toggleDebugMode() ;

void xamineELFFile();

void quit();

void PrintSectionNames();

int Current_fd = -1;

int d_flag = 0;

struct stat sb;

Elf32_Ehdr *ehdr;

void *addr;

int main(int argc, char** argv){
    struct menuItem menu[] = { {"Toggle Debug Mode", toggleDebugMode}, {"Examine ELF File", xamineELFFile},
                               {"Print Section Names", PrintSectionNames}, {"Quit", quit}, {NULL , NULL}};
    int i;
    int choose;

    while (1){
        printf("Choose action: \n");
        for(i = 0; menu[i].action != NULL ; i++){
            printf("%d-%s \n", i, menu[i].action);
        }

        char input_line[name_size];
        fgets(input_line, name_size, stdin);
        sscanf(input_line, "%d", &choose);

        if (choose >= 0 && choose <= menu_size)
            menu[choose].fun();
        else
            printf("Not an option. please select again\n\n ");
    }

}

void PrintSectionNames() {
    char typeString[100];

    if (Current_fd == -1){
        perror("Currentfd is invalid");
        return;
    }

    Elf32_Shdr * section_header;

    section_header= (Elf32_Shdr*)(addr + ehdr->e_shoff);

    char *section_header_table = (char*)(addr + section_header[ehdr->e_shstrndx].sh_offset);

    printf("%s\t %-20s %s\t %s\t %s\t %s", "index", "name", "address", "offset", "size", "type");

    if (d_flag)
        printf("\t %s\n", "DEBUG: offset");
    else
        printf("\n");

    int i;
    for(i = 0; i < ehdr->e_shnum; i++){
        switch (section_header[i].sh_type) {
            case SHT_NULL:                          /* Section header table entry unused */
                strcpy(typeString,"NULL");
                break;
            case SHT_PROGBITS:                      /* Program data */
                strcpy(typeString,"PROGBITS");
                break;
            case SHT_SYMTAB:                        /* Symbol table */
                strcpy(typeString,"SYMTAB");
                break;
            case SHT_STRTAB:                        /* String table */
                strcpy(typeString,"STRTAB");
                break;
            default:
                strcpy(typeString,"0000000");
                break;
        }
        printf("[%d]\t %-20s %08X\t %06X\t %d\t %s", i, section_header_table + section_header[i].sh_name, section_header[i].sh_addr,
               section_header[i].sh_offset, section_header[i].sh_size, typeString);
        if (d_flag)
            printf("\t %d\n", section_header[ehdr->e_shstrndx].sh_offset+section_header[i].sh_offset);
        else
            printf("\n");
    }
}

void xamineELFFile() {
    char buffer[name_size];

    char filename[name_size];

    printf("please enter an ELF file name: \n" );

    fgets(buffer ,100, stdin);
    sscanf(buffer, "%s", filename);

    if(Current_fd != -1){
        close(Current_fd);
    }

    Current_fd = open(filename, O_RDWR, 0677);

    /*Current_fd = open("chez", O_RDWR, 0677);*/
    if (Current_fd == -1) {
        perror("Error - could not open file");
        return;
    }

    if (fstat(Current_fd, &sb) == -1){
        close(Current_fd);
        Current_fd = -1;
        perror("fstat");
        exit(Failed_Exit);
    }

    if((addr = mmap(0, (size_t) sb.st_size, PROT_READ, MAP_PRIVATE, Current_fd, 0)) < 0){
        close(Current_fd);
        Current_fd= -1;
        perror("mmap");
        exit(Failed_Exit);
    }


    ehdr = (Elf32_Ehdr*)addr;

    if(ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1 || ehdr->e_ident[EI_MAG2] != ELFMAG2 
       || ehdr->e_ident[EI_MAG3] != ELFMAG3 || ehdr->e_ident[EI_CLASS] != ELFCLASS32){
        close(Current_fd);
        Current_fd = -1;
        perror("file isn't ELF");
        exit(Failed_Exit);
    }

    printf("Bytes 1,2,3 of the magic number: \t%c, %c, %c\n", ehdr->e_ident[EI_MAG1], ehdr->e_ident[EI_MAG2], ehdr->e_ident[EI_MAG3]);

    printf("Data encoding byte index: \t");

    switch(ehdr->e_ident[EI_DATA]){
        case ELFDATA2LSB:
            printf("Data: 2's complement, little endian\n");
            break;
        case ELFDATA2MSB:
            printf("Data: 2's complement, big endian\n");
            break;
        case ELFDATANONE:
            perror("Invalid data encoding\n");
            exit(Failed_Exit);
        default:
            perror("Invalid Permater");
            exit(Failed_Exit);
    }

    printf("Entry point of program: \t%08x\n", ehdr->e_entry);

    printf("The file offset in which the section header table resides: \t%d\n", ehdr->e_shoff);

    printf("The number of section header entries: \t%d\n", ehdr->e_shnum);

    printf("The size of each section header entry: \t%d\n", ehdr->e_shentsize);

    printf("The file offset in which the program header table resides: \t%d\n", ehdr->e_phoff);

    printf("The number of program header entriess: \t%d\n", ehdr->e_phnum);

    printf("The size of each program header entry: \t%d\n\n", ehdr->e_phentsize);
}

void toggleDebugMode() {
    if(d_flag){
        d_flag = 0;
        fprintf(stderr, "Debug flag now off\n\n");

    }else{
        d_flag = 1;
        fprintf(stderr, "Debug flag now on\n\n");
    }

}

void quit(){
    if(munmap(addr, (size_t) sb.st_size) < 0){
        perror("munmap failed");
    }
    close(Current_fd);
    if (d_flag)
        printf ("quitting");
    exit(Success_Exit);
}


