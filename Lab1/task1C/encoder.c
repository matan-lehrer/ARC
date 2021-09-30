#include <stdio.h>
#include <string.h>


const int diffConst = 32;

int uperCase(int char1);

int encrypt(int char1, char i, int i1);

void mission0();

void mission1A();

void mission1B(char sign, int key);

void mission1C(char sign, int key, char inputFileName[50]);


int main(int argc, char **argv) {
    char sign = '+';
    int key =  0;
    int num, i;

    int inputF = 0;
    char inputFileName[50] = "myFile.txt";

    for(i=1; i<argc; i++) {
        if(argv[i][0] == '-' || argv[i][0] == '+') {
            sign = argv[i][0];
            num = argv[i][1];
            if (argv[i][1] == 'i') {
                inputF = 1;
                if (argv[i + 1] != 0 && argv[i + 1] != NULL) {
                    strcpy(inputFileName, argv[++i]);
                }
            }
            else if('A' <= num && num <= 'Z') {
                key = num - 'A' + 1;
            }
            else if ('a' <= num && num <= 'z') {
                key = num - 'a' + 1;
            }
            else  {
                printf("Error value");
                return 1;
            }
        }  else {
            printf("invalid parameter - %s\n", argv[i]);
            return 1;
        }
    }

    if(inputF){
        mission1C(sign, key, inputFileName);
    }else mission1B(sign, key);

    return 0;
}

void mission0() {
    int char1;
    puts("Mission 0");
    while ((char1 = getchar()) != '\n'){
        printf("%c", char1);
    }
    puts("\n");
}


void mission1A() {
    int char1;
    while ((char1 = getchar()) != '\n'){
        char1 = uperCase(char1);
        printf("%c", char1);
    }
    puts("\n");
}


void mission1B(char sign, int key) {
    int char1;
    while ((char1 = getchar()) != '\n'){
        char1 = uperCase(char1);
        char1 = encrypt(char1, sign, key);
        printf("%c", char1);
    }
    puts("\n");
}


void mission1C(char sign, int key, char inputFileName[50]) {
    FILE* file = fopen(inputFileName, "r");
    if(file == NULL){
        printf("Failed opening the file.Exiting!\n");
        return;
    }
    int char1;
    while((char1 = fgetc(file)) != EOF){
        char1 = uperCase(char1);
        char1 = encrypt(char1, sign, key);
        printf("%c", char1);
    }
    puts("\n");
    fclose(file);

}

int encrypt(int c1, char value, int key) {
    if(key == 0 || c1 < 'A' || 'Z' < c1){  }
    else if (value == '+'){
        c1 = c1 + key;
        while (c1 > 'Z')
            c1 = c1 - 26;
    }
    else if (value == '-'){
        c1 = c1 - key;
        while (c1 < 'A'){
            c1 = c1 + 26;
        }
    }else
        puts("Error with loading the values");
    return c1;
}

int uperCase(int char1) {
    if ('a' <= char1 && char1 <= 'z' )
        char1 = char1 - diffConst ;
    return char1;
}