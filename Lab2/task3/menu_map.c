#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

char encrypt(char c) { /* Gets a char c and returns its encrypted form by adding 3 to its value.
          If c is not between 0x20 and 0x7E it is returned unchanged */
    if(0x20 <= c && c <= 0x7E){
        return c + 3;
    }
    return c;
}
char decrypt(char c) { /* Gets a char c and returns its decrypted form by reducing 3 to its value.
            If c is not between 0x20 and 0x7E it is returned unchanged */
    if((char)0x20 <= c &&  c <= (char) 0x7E){
        return c - 3;
    }
    return c;
}
char xprt(char c) { /* xprt prints the value of c in a hexadecimal representation followed by a
           new line, and returns c unchanged. */
    printf("%#04x\n", c);
    return c;

}
char cprt(char c) { /* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line.
                    Otherwise, cprt prints the dot ('.') character. After printing, cprt returns the value of c unchanged. */
    if(0x20 <= c && c <= 0x7E){
        printf("%c\n", c);
    }else
        printf(".\n");
    return c ;

}
char my_get(char c){  /* Ignores c, reads and returns a character from stdin using fgetc. */
    int i = fgetc(stdin);
    if(i == '\n')
        return '\0';
    return i;
}

char quit(char c){  /* Gets a char c, and ends the program using c as the return value */
    exit(0);
}
char censor(char c) {
    if(c == '!')
        return '.';
    else
        return c;
}

char* map(char *array, int array_length, char (*f) (char)){
    long int i = 0;
    for (i = 0; i < array_length; i++) {
        array[i] = f(array[i]);
        if(array[i] == '\0'){
            return array;
        }
    }
    return array;
}

int main(int argc, char **argv){
    struct fun_desc {
        char *name;
        char (*fun)(char);
    };

    struct fun_desc menu[] =
            { { "Censor", censor }, { "Encrypt", encrypt },{ "Decrypt", decrypt },{ "Print hex", xprt },{ "Print string", cprt },{ "Get string", my_get },{ "Quit", quit }, { NULL, NULL } };

    int base_len = 5;
    char* arr1 = (char*)(malloc(base_len*sizeof(char)));

    while (1){
printf("Please choose a function:\n");
        long int j = 0;
        for (j = 0; j < 6; j++) {
            printf("%d) %s\n", j, menu[j].name);
        }
        printf("Option:\n");
        char *char1[50];
        gets(char1);
        char i = char1[0];

        if('0' <= i && i <= '6'){
            printf("Within bounds\n");
            if(i == '0')
                arr1 = map(arr1, base_len, menu[0].fun);
            if(i == '1')
                arr1 = map(arr1, base_len, menu[1].fun);
            if(i == '2')
                arr1 = map(arr1, base_len, menu[2].fun);
            if(i == '3')
                arr1 = map(arr1, base_len, menu[3].fun);
            if(i == '4')
                arr1 = map(arr1, base_len, menu[4].fun);
            if(i == '5')
                arr1 = map(arr1, base_len, menu[5].fun);
            if(i == '6')
                arr1 = map(arr1, base_len, menu[6].fun);
            printf("DONE.\n\n");

        }
    }

}