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

void PrintSymbols();

int Current_fd = -1;

int d_flag = 0;

struct stat sb;

Elf32_Ehdr *ehdr;

void *addr;

int main(int argc, char** argv){
    struct menuItem menu[] = { {"Toggle Debug Mode", toggleDebugMode}, {"Examine ELF File", xamineELFFile},
                               {"Print Section Names", PrintSectionNames}, {"Print Symbols", PrintSymbols}, {"Quit", quit}, {NULL , NULL}};
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

int validSection(int section){
    if(section == SHN_LORESERVE  /* End of processor-specific */ || section == SHN_HIPROC  /* End of processor-specific */ ||
            section == SHN_ABS /* Associated symbol is absolute */ || section == SHN_COMMON /* Associated symbol is common */ ||
            section == SHN_HIRESERVE /* End of reserved indices */)
        return 0;
    return 1;
}

void PrintSymbols() {
    if (Current_fd == -1) {
        perror("Currentfd is invalid");
        return;
    }

    Elf32_Shdr *section_header;

    section_header = (Elf32_Shdr *) (addr + ehdr->e_shoff);
    char *sections_header_table = (char*)(addr + section_header[ehdr->e_shstrndx].sh_offset);

    int i;
    for (i = 0; i < ehdr->e_shnum; i++) {
        if (section_header[i].sh_type == SHT_SYMTAB /* Symbol table */ || section_header[i].sh_type == SHT_DYNSYM  /* Dynamic linker symbol table */) {
            Elf32_Sym *symbol_table_entry = (Elf32_Sym *) (addr + section_header[i].sh_offset);    /* Section file offset */
            int size_of_table = section_header[i].sh_size / sizeof(Elf32_Sym);     /* Section size in bytes /Symbol table entry.  */
            char *section_header_table = (char *) (section_header[section_header[i].sh_link].sh_offset + addr);

            if (d_flag)
                printf("\nthe size of symbol table: %d , \t  the number of sybmols therein: %d ", section_header[i].sh_size, size_of_table);

            printf("\n%s\t %s\t\t %s\t %s\t %s\n", "[index]", "value", "section_index", "section_name", "symbol_name");
            int j;
            for (j = 0; j < size_of_table; j++) {
                if (validSection(symbol_table_entry[j].st_shndx))
                    printf("[%d]\t %08X\t %d\t\t %s\t\t %s\n", j, symbol_table_entry[j].st_value, symbol_table_entry[j].st_shndx,
                           sections_header_table + section_header[symbol_table_entry[j].st_shndx].sh_name,
                           section_header_table + symbol_table_entry[j].st_name);
                 else   printf("[%d]\t %08X\t %d\t\t %s\t\t %s\n", j, symbol_table_entry[j].st_value, symbol_table_entry[j].st_shndx, "",
                           section_header_table + symbol_table_entry[j].st_name);
            }
        }
    }

    printf("\n");
}

void PrintSectionNames() {
    if (Current_fd == -1){
        perror("Currentfd is invalid");
        return;
    }

    char typeString[name_size];

    Elf32_Shdr * section_header;

    section_header= (Elf32_Shdr*)(addr + ehdr->e_shoff);

    char *section_header_table = (char*)(addr + section_header[ehdr->e_shstrndx].sh_offset);

    printf("%s\t\t %s\t\t\t %s\t\t\t %s\t\t\t %s\t\t\t %s", "index", "name", "address", "offset", "size", "type");

    if (d_flag)
        printf("\t\t\t %s\n", "DEBUG: offset");
    else
        printf("\n");
    
    char *typeArray[]=
                    {"NULL","PROGBITS","SYMTAB","STRTAB","RELA","HASH","DYNAMIC","NOTE","NOBITS",
                     "REL","SHLIB","DYNSYM","INIT_ARRAY","FINI_ARRAY","PREINIT_ARRAY",
                     "GROUP","SYMTAB_SHNDX","NUM"
                    };
                    
    int i;
    for(i = 0; i < ehdr->e_shnum; i++){
        switch (section_header[i].sh_type) {
            case SHT_LOOS:
                strcpy(typeString, "LOOS");
                break;
            case SHT_GNU_ATTRIBUTES:
                strcpy(typeString, "GNU_ATTRIBUTES");
                break;
            case SHT_GNU_HASH:
                strcpy(typeString, "GNU_HASH");
                break;
            case SHT_GNU_LIBLIST:
                strcpy(typeString, "GNU_LIBLIST");
                break;
            case SHT_CHECKSUM:
                strcpy(typeString, "CHECKSUM");
                break;
            case SHT_LOSUNW:
                strcpy(typeString, "LOSUNW");
                break;
            case SHT_SUNW_COMDAT:
                strcpy(typeString, "SUNW_COMDAT");
                break;
            case SHT_SUNW_syminfo:
                strcpy(typeString, "SUNW_syminfo");
                break;
            case SHT_GNU_verdef:
                strcpy(typeString, "VERDEF");
                break;
            case SHT_GNU_verneed:
                strcpy(typeString, "VERNEED");
                break;
            case SHT_GNU_versym:
                strcpy(typeString, "VERSYM");
                break;
            case SHT_LOPROC:
                strcpy(typeString, "LOPROC");
                break;
            case SHT_HIPROC:
                strcpy(typeString, "HIPROC");
                break;
            case SHT_LOUSER:
                strcpy(typeString, "LOUSER");
                break;
            case SHT_HIUSER:
                strcpy(typeString, "HIUSER");
                break;
            default:
                strcpy(typeString, "0000000");
                break;
        }
        if(section_header[i].sh_type <= 19){
        printf("[%d]\t\t %s\t\t\t %08X\t\t\t %06X\t\t\t %d\t\t\t %s", i, section_header_table + section_header[i].sh_name, section_header[i].sh_addr,
               section_header[i].sh_offset, section_header[i].sh_size, typeArray[section_header[i].sh_type]);
        }else{
        printf("[%d]\t\t %s\t\t\t %08X\t\t\t %06X\t\t\t %d\t\t\t %s", i, section_header_table + section_header[i].sh_name, section_header[i].sh_addr,
               section_header[i].sh_offset, section_header[i].sh_size, typeString);
            
        }
            
        if (d_flag)
            printf("\t\t %d\n", section_header[ehdr->e_shstrndx].sh_offset+section_header[i].sh_offset);
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
            printf("2's complement, little endian\n");
            break;
        case ELFDATA2MSB:
            printf("2's complement, big endian\n");
            break;
        case ELFDATANONE:
            perror("Invalid data encoding\n");
            exit(Failed_Exit);
        default:
            perror("Invalid Permater");
            exit(Failed_Exit);
    }

    printf("Entry point of program: \t0x%x\n", ehdr->e_entry);

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


