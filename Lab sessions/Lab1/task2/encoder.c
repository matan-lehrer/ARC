#include <stdio.h>
#include <string.h>


const int diffConst = 32;

int uperCase(int char1);

int encrypt(int char1, char i, int i1);

void output( int inputF, char sign, int key, char inputFileName[50], char outputFileName[50]);

void simple( int inputF, char sign, int key, char inputFileName[50]);

int main(int argc, char **argv) {
    char sign = '+';
    int key =  0;
    int num, i;

    int inputF = 0;
    int outputF = 0;
    char inputFileName[50] = "myFile.txt";
    char outputFileName[50] =  "outputFile.txt";

    for(i=1; i<argc; i++) {
        if(argv[i][0] == '+') {
            sign = argv[i][0];
            num = argv[i][1];
            if('A' <= num && num <= 'Z') {
                key = num - 'A' + 1;
            }
            else if ('a' <= num && num <= 'z') {
                key = num - 'a' + 1;
            }
            else  {
                printf("Error invalid parameter - %s\n", argv[i]);
                return 1;
            }
        } else if(argv[i][0] == '-') {
            if (argv[i][1] == 'i') {
                inputF = 1;
                if (argv[i + 1] != 0 && argv[i + 1] != NULL) {
                    strcpy(inputFileName, argv[++i]);
                }
            } else if (argv[i][1] == 'o') {
                outputF = 1;
            } else {
                sign = argv[i][0];
                num = argv[i][1];
            }
            if ('A' <= num && num <= 'Z') {
                key = num - 'A' + 1;
            } else if ('a' <= num && num <= 'z') {
                key = num - 'a' + 1;
            }else  {
                printf("Error invalid parameter - %s\n", argv[i]);
                return 1;
            }
        }
        else {
            printf("invalid parameter - %s\n", argv[i]);
            return 1;
        }
    }

    if(outputF){
        puts("Enter output file: ");
        int j = 0;
        int char1;
        for ( j = 0; j < 50; ++j) {
            if ((char1 = getchar()) != '\n')
                outputFileName[j] = char1;
            else {
                outputFileName[j] = '\0';
                j = 50;
            }
        }
        output(inputF, sign, key, inputFileName, outputFileName);
    }else simple(inputF, sign, key, inputFileName);

    return 0;
}

void output( int inputF, char sign, int key, char inputFileName[50], char outputFileName[50]) {
    FILE *outputFile = fopen(outputFileName, "w");
    int char1;
    if (outputFile == NULL) {
        printf("Failed opening the file.Exiting!\n");
        return;
    }

    if (inputF) {
        FILE *file = fopen(inputFileName, "r");
        if (file == NULL) {
            printf("Failed opening the file.Exiting!\n");
            return;
        }
        while ((char1 = fgetc(file)) != EOF) {
            char1 = uperCase(char1);
            char1 = encrypt(char1, sign, key);
            fputc(char1, outputFile);
        }
        fclose(file);
    } else {
        while ((char1 = getchar()) != '\n') {
            char1 = uperCase(char1);
            char1 = encrypt(char1, sign, key);
            fputc(char1, outputFile);
        }
    }
    fclose(outputFile);
}

void simple( int inputF, char sign, int key, char inputFileName[50]) {
    int char1;
    if (inputF) {
        FILE *file = fopen(inputFileName, "r");
        if (file == NULL) {
            printf("Failed opening the file.Exiting!\n");
            return;
        }
        while ((char1 = fgetc(file)) != EOF) {
            char1 = uperCase(char1);
            char1 = encrypt(char1, sign, key);
            printf("%c", char1);
        }
        fclose(file);
    } else {
        while ((char1 = getchar()) != '\n') {
            char1 = uperCase(char1);
            char1 = encrypt(char1, sign, key);
            printf("%c", char1);
        }
    }
    printf("\n");
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