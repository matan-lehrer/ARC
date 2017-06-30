#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/types.h> 
#include <unistd.h>
#include <math.h>
#define INPUT_SIZE 8

char            debugFlag       = 0;
char            size            = 1;
char*           filename        = 0;
unsigned char*  data_pointer    = NULL;

int checkNumber(char c) {
    if ((c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F'))
        return 1;
    return 0;
}

int checkValidity(char* str, int numOfParams) {
    int i;
    int counter = 1;
    if (str[0]==' ')
        return 0;
    for(i=1; i<strlen(str)-1; i++) {
        if (str[i]==' ') {
            counter++;
        }
        else if (!checkNumber(str[i])) {
            return 0;
        }
    }
    if (counter==numOfParams)
        return 1;
    return 0;
}
 
void toggleDebugMode() {
    debugFlag = 1-debugFlag;
    if (debugFlag)
        printf("Debug flag now on\n");
    else
        printf("Debug flag now off\n");
}

void setFileName() {
    if (filename)
        free(filename);
    char *name = (char*) malloc(100);
    printf("Insert a file name: ");
    fgets(name, 100, stdin);
    int len = strlen(name)-1;
    if (name[len] == '\n')
        name[len] = '\0';
    filename = name;
    if (debugFlag)
        printf("Debug: file name set to %s\n", filename);
}

void setUnitSize() {
    char *sizeNum = (char*) malloc(INPUT_SIZE);
    printf("Insert a number: ");
    fgets(sizeNum, INPUT_SIZE, stdin);
    if (strcmp(sizeNum, "1\n")==0 || strcmp(sizeNum, "2\n")==0 || strcmp(sizeNum, "4\n")==0) {
        size = atoi(sizeNum);
        if (debugFlag)
            printf("Debug: set size to %d\n", size);
    }
    else
        printf("Error: illegal size\n");
}

void fileDisplay() {
    if (!filename) {
        printf("Error: no file name inserted\n");
        return;
    }
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error - could not open file");
        return;
    }
    int location, length, i, j;
    char userInput[200];
    printf("Please enter <location> <length>\n");
    fgets(userInput, 200, stdin);
    if (!checkValidity(userInput, 2)) {
        printf("Illegal input\n");
        return;
    }
        
    sscanf(userInput, "%x %d", &location, &length);
    if (debugFlag) 
        printf("Debug - those are the values inserted:\nlocation: 0x%x\nlength: %d\n\n", location, length);
    unsigned char* buffer = (unsigned char*) malloc(length*size);
    lseek(fd, location, SEEK_SET);
    read(fd, buffer, length*size);
    close(fd);
    int rep[length];
    for (i=0; i<length; i++) {
        rep[i] = 0;
        for (j=size-1; j>=0; j--)
            rep[i] += buffer[size*i+j]*pow(256,j);
    }
    printf("Hexadecimal Representation:\n");
    for (i=0; i<length; i++) {
        switch(size) {
            case 1:
                printf("%02x", rep[i]);
                break;
            case 2:
                printf("%04x", rep[i]);
                break;
            case 4:
                printf("%08x", rep[i]);
        }
        if (i < length-1)
            printf(" ");
    }
    printf("\n");
    printf("Decimal Representation:\n");
    for (i=0; i<length; i++) {
        printf("%d", rep[i]);
        if (i < length-1)
            printf(" ");
    }
    printf("\n");
    if (buffer)
        free(buffer);
}

void loadIntoMemory() {
    if (!filename) {
        printf("Error: no file name inserted\n");
        return;
    }
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error - could not open file");
        return;
    }
    int location, length, i;
    char userInput[200];
    printf("Please enter <location> <length>\n");
    fgets(userInput, 200, stdin);
    if (!checkValidity(userInput, 2)) {
        printf("Illegal input\n");
        return;
    }
    sscanf(userInput, "%x %d", &location, &length);
    if (data_pointer)
        free(data_pointer);
    length *= size;
    data_pointer = (unsigned char*) malloc(length);
    lseek(fd, location, SEEK_SET);
    read(fd, data_pointer, length);
    close(fd);
    if (debugFlag) {
        printf("Loaded %d bytes into memory address 0x%08x from file %s starting from position 0x%x\n", length, data_pointer, filename, location);
        printf("Those are the bytes loaded:\n");
        for (i=0; i<length; i++) {
            printf("%02x", data_pointer[i]);
            if (i < length-1)
                printf(" ");
        }
        printf("\n");
    }
}

void saveIntoFile() {
    if (!filename) {
        printf("Error: no file name inserted\n");
        return;
    }
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Error - could not open file");
        return;
    }
    int sourceAddress, targetLocation, length, fileSize, i;
    char userInput[200];
    printf("Please enter <source-address> <target-location> <length>\n");
    fgets(userInput, 200, stdin);
    if (!checkValidity(userInput, 3)) {
        printf("Illegal input\n");
        return;
    }
    sscanf(userInput, "%x %x %d", &sourceAddress, &targetLocation, &length);
    fileSize = lseek(fd, 0, SEEK_END);
    if (targetLocation+length > fileSize) {
        printf("Error: target-location + length is greater then the size of the file %s\n", filename);
        close(fd);
        return;
    }
    if (sourceAddress == 0) sourceAddress = data_pointer;
    lseek(fd, targetLocation, SEEK_SET);
    write(fd, sourceAddress, length);
    if (debugFlag) {
        printf("Wrote %d bytes from memory address 0x%08x into file %s starting from position 0x%x\n", length, sourceAddress, filename, targetLocation);
        unsigned char buf[length];
        lseek(fd, targetLocation, SEEK_SET);
        read(fd, buf, length);
        printf("Those are the bytes written:\n");
        for (i=0; i<length; i++) {
            printf("%02x", buf[i]);
            if (i < length-1)
                printf(" ");
        }
        printf("\n");
    }
    close(fd);
}

void fileModify() {
    if (!filename) {
        printf("Error: no file name inserted\n");
        return;
    }
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Error - could not open file");
        return;
    }
    int location, val, fileSize, i;
    char userInput[200];
    printf("Please enter <location> <val>\n");
    fgets(userInput, 200, stdin);
    if (!checkValidity(userInput, 2)) {
        printf("Illegal input\n");
        return;
    }
    sscanf(userInput, "%x %x", &location, &val);
    fileSize = lseek(fd, 0, SEEK_END);
    if (location+size > fileSize) {
        printf("Error: location + unitsize is greater then the size of %s\n", filename);
        close(fd);
        return;
    }
    if (val > pow(256,size)-1) {
        printf("Error: val is not compatible with unit size %d\n", size);
        close(fd);
        return;
    }
    lseek(fd, location, SEEK_SET);
    unsigned char buffer[size];
    for (i=0; i<size; i++) {
        buffer[i] = val%256;
        val = val/256;
    }
    write(fd, buffer, size);
    if (debugFlag) {
        printf("Wrote %d bytes into file %s starting from position 0x%x\n", size, filename, location);
        unsigned char buf[size];
        lseek(fd, location, SEEK_SET);
        read(fd, buf, size);
        printf("Those are the bytes written:\n");
        for (i=0; i<size; i++) {
            printf("%02x", buf[i]);
            if (i < size-1)
                printf(" ");
        }
        printf("\n");
    }
    close(fd);
}

void quit() {
    if (filename)
        free(filename);
    if (data_pointer)
        free(data_pointer);
    if (debugFlag)
        printf("quitting\n");
    exit(0);
}
 
int main(int argc, char **argv){
    struct fun_desc {
        char *name;
        void (*fun)();
    };
    struct fun_desc menu[] = {{"Toggle Debug Mode", toggleDebugMode}, {"Set File Name", setFileName}, {"Set Unit Size", setUnitSize}, {"File Display", fileDisplay}, {"Load Into Memory", loadIntoMemory}, {"Save Into File", saveIntoFile}, {"File Modify", fileModify}, {"Quit", quit}, {NULL, NULL}};
    int i, functionsAmount, choise;
    for (functionsAmount=0; menu[functionsAmount].name; functionsAmount++) {}
    while (1) {
        if (debugFlag) {
            printf("Debug:\nUnit Size: %d\nFile Name: %s\nBuffer Address: 0x%08x\n\n", size, filename, data_pointer);
        }
        printf("Choose action:\n");
        for (i=0; i<functionsAmount; i++) {
            printf("%d-%s\n", i, menu[i].name);
        }
        printf("Option: ");
        char c[] = "";
        fgets(c, INPUT_SIZE, stdin);
        if (c[0]=='\n') {
            printf("Invalid input\n");
            exit(1);
        }
        int k;
        for (k=0; c[k]!='\0' && c[k]!='\n'; k++) {
            if (c[k] > '9' || c[k] < '0') {
                printf("Invalid input\n");
                exit(1);
            }
        }
        choise = atoi(c);
        if (choise >=functionsAmount) {
            printf("Not within bounds\n");
            exit(1); 
        }
        menu[choise].fun();
        printf("\n");
    }
    return 0;
}