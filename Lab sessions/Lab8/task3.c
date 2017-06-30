#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <elf.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>

#define size_name 100

#define size_menu 8

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

void LinkCheck();

void RelocationTablesRaw();

void RelocationTablesSemantic();

int containsStart(char *string);

int Current_fd = -1;

int d_flag = 0;

struct stat sb;

Elf32_Ehdr *ehdr;

void *addr;

int main(int argc, char** argv){
    struct menuItem menu[] = { {"Toggle Debug Mode", toggleDebugMode}, {"Examine ELF File", xamineELFFile}, {"Print Section Names", PrintSectionNames},
                               {"Print Symbols", PrintSymbols}, {"Link Check", LinkCheck}, {"Relocation Tables - Raw", RelocationTablesRaw},
                               {"Relocation Tables - Semantic", RelocationTablesSemantic}, {"Quit", quit}, {NULL , NULL}};
    int i, choose;
    while (1){
        printf("Choose action: \n");
        for(i = 0; menu[i].action != NULL ; i++){
            printf("%d-%s \n", i, menu[i].action);
        }

        char input_line[size_name];
        fgets(input_line, size_name, stdin);
        sscanf(input_line, "%d", &choose);

        if (choose >= 0 && choose <= size_menu)
            menu[choose].fun();
        else
            printf("Not an option. please select again\n\n ");
    }
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

void xamineELFFile() {
    char buffer[size_name];

    char filename[size_name];

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

void PrintSectionNames() {
    if (Current_fd == -1){
        perror("Currentfd is invalid");
        return;
    }

    char typeString[size_name];

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

int validSection(int section){
    if(section == SHN_LORESERVE  /* Start of reserved indices */ || section == SHN_HIPROC  /* End of processor-specific */ ||
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

void LinkCheck() {
    if (Current_fd == -1) {
        perror("Currentfd is invalid");
        return;
    }

    Elf32_Shdr *section_header;

    section_header = (Elf32_Shdr *) (addr + ehdr->e_shoff);

    int i;
    for (i = 0; i < ehdr->e_shnum; i++) {
        if (section_header[i].sh_type == SHT_SYMTAB /* Symbol table */ || section_header[i].sh_type == SHT_DYNSYM  /* Dynamic linker symbol table */) {
            Elf32_Sym *symbol_table_entry = (Elf32_Sym *) (addr + section_header[i].sh_offset);    /* Section file offset */
            int size_of_table = section_header[i].sh_size / sizeof(Elf32_Sym);     /* Section size in bytes /Symbol table entry.  */
            char *section_header_table = (char *) (section_header[section_header[i].sh_link].sh_offset + addr);

            int j;
            for (j = 0; j < size_of_table; j++) {
                if(d_flag)
                    printf("[%d]\t %08X\t %s\n\n", j, symbol_table_entry[j].st_value, section_header_table + symbol_table_entry[j].st_name);

                if (containsStart(section_header_table + symbol_table_entry[j].st_name)) {
                    printf("_start check: PASSED\n\n");
                    return;
                }

            }
        }
    }
    printf("_start check: FAILED\n\n");
}

int containsStart(char *symbol_name) {
    if(strcmp(symbol_name, "_start"))
        return 0;
    return 1;
}


void RelocationTablesRaw() {
    if (Current_fd == -1){
        perror("Currentfd is invalid");
        return;
    }

    Elf32_Shdr * section_header;

    section_header= (Elf32_Shdr*)(addr + ehdr->e_shoff);

    char *section_header_table = (char*)(addr + section_header[ehdr->e_shstrndx].sh_offset);

    d_flag ? printf("\t %s\n", "DEBUG: offset") : printf("\n");

    int i;
    for(i = 0; i < ehdr->e_shnum; i++){
        if (section_header[i].sh_type == SHT_REL){
            if (d_flag)
                printf("[%d]\t %s\t %08X\t %06X\t %d\t %s\n\n", i, section_header_table + section_header[i].sh_name, section_header[i].sh_addr,
                       section_header[i].sh_offset, section_header[i].sh_size, "REL");
            Elf32_Rel* relocationTableEntry = (Elf32_Rel*) (addr + section_header[i].sh_offset);

            int size_of_table = section_header[i].sh_size / sizeof(Elf32_Rel);     /* Section size in bytes /Symbol table entry.  */

            printf("%s\t\t %s\n", "offset", "info");

            int j;
            for (j = 0; j < size_of_table; j++) {
                printf("%08x\t %08x\n", relocationTableEntry[j].r_offset, relocationTableEntry[j].r_info);
            }
            printf("\n");
        }
    }
}

void RelocationTablesSemantic() {
    if (Current_fd == -1){
        perror("Currentfd is invalid");
        return;
    }

    Elf32_Shdr * section_header;

    section_header= (Elf32_Shdr*)(addr + ehdr->e_shoff);
    char *section_header_sym_table = NULL;
    Elf32_Sym *symbol_table_entry = NULL;
    int tableNum;
    tableNum = ehdr->e_type == 1 ? SHT_SYMTAB : SHT_DYNSYM;
    int i;
    for (i = 0; i < ehdr->e_shnum; i++) {
        if (section_header[i].sh_type == tableNum)  {
            symbol_table_entry = (Elf32_Sym *) (addr + section_header[i].sh_offset);    /* Section file offset */
            section_header_sym_table = (char *) (section_header[section_header[i].sh_link].sh_offset + addr);
        }
    }


    char *section_header_table = (char*)(addr + section_header[ehdr->e_shstrndx].sh_offset);

    if (d_flag)
        printf("\t %s\n", "DEBUG: offset");
    else
        printf("\n");

    for(i = 0; i < ehdr->e_shnum; i++){
        if (section_header[i].sh_type == SHT_REL){
            if (d_flag)
                printf("[%d]\t %s\t %08X\t %06X\t %d\t %s\n\n", i, section_header_table + section_header[i].sh_name, section_header[i].sh_addr,
                       section_header[i].sh_offset, section_header[i].sh_size, "REL");
            Elf32_Rel* relocationTableEntry = (Elf32_Rel*) (addr + section_header[i].sh_offset);

            int size_of_table = section_header[i].sh_size / sizeof(Elf32_Rel);     /* Section size in bytes /Symbol table entry.  */

            char *typeArray[]=
                    {"R_386_NONE","R_386_32","R_386_PC32","R_386_GOT32","R_386_PLT32","R_386_COPY","R_386_GLOB_DAT","R_386_JMP_SLOT","R_386_RELATIVE",
                     "R_386_GOTOFF","R_386_GOTPC","R_386_32PLT","R_386_TLS_TPOFF","R_386_TLS_IE","R_386_TLS_GOTIE",
                     "R_386_TLS_LE","R_386_TLS_GD","R_386_TLS_LDM","R_386_16","R_386_PC16","R_386_8","R_386_PC8",
                     "R_386_TLS_GD_32","R_386_TLS_GD_PUSH","R_386_TLS_GD_CALL","R_386_TLS_GD_POP","R_386_TLS_LDM_32",
                     "R_386_TLS_LDM_PUSH","R_386_TLS_LDM_CALL","R_386_TLS_LDM_POP","R_386_TLS_LDO_32","R_386_TLS_IE_32",
                     "R_386_TLS_LE_32","R_386_TLS_DTPMOD32","R_386_TLS_DTPOFF32","R_386_TLS_TPOFF32","R_386_NUM"
                    };

            printf("%s\t\t %s\t\t %s\t\t %s\t %s\n", "Offset", "Info", "Type" ,  "Sym.Value", "Sym. Name");

            int j;
            for (j = 0; j < size_of_table; j++) {
                int type = ELF32_R_TYPE(relocationTableEntry[j].r_info);
                int symbolNAME = ELF32_R_SYM(relocationTableEntry[j].r_info);
                char* name = section_header_sym_table + symbol_table_entry[symbolNAME].st_name;
                if (strlen(name) == 0)
                    name = section_header_table + section_header[symbolNAME].sh_name;
                int value = symbol_table_entry[symbolNAME].st_value;
                printf("%08x\t %08x\t %s\t %08x\t %s\n", relocationTableEntry[j].r_offset, relocationTableEntry[j].r_info, typeArray[type], value, name);
            }
            printf("\n");
        }
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


