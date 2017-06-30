#include <stdio.h>
#include <string.h>
#include <malloc.h>

char encrypt(char c) { /* Gets a char c and returns its encrypted form by adding 3 to its value.
          If c is not between 0x20 and 0x7E it is returned unchanged */
    if(0x20 <= c && c <= 0x7E){
        return c;
    }
    return c + 3;
}
char decrypt(char c) { /* Gets a char c and returns its decrypted form by reducing 3 to its value.
            If c is not between 0x20 and 0x7E it is returned unchanged */
    if((char)0x20 <= c &&  c <= (char) 0x7E){
        return c;
    }
    return c - 3;
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
    return fgetc(stdin);
}

char quit(char c){  /* Gets a char c, and ends the program using c as the return value */
    exit(c);
}
char censor(char c) {
    if(c == '!')
        return '.';
    else
        return c;
}

char* map(char *array, int array_length, char (*f) (char)){
    char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
    long int i = 0;
    for (i = 0; i < array_length; i++)
        mapped_array[i] = f(array[i]);
    return mapped_array;
}

int main(int argc, char **argv){

    int base_len = 5;
    char *arr1;
    arr1 = (char*) malloc(sizeof(char) * base_len);
    arr1 = "Hey!";
    while (1){
        printf("Please choose a function:\n"
                       "0) Censor\n"
                       "1) Encrypt\n"
                       "2) Decrypt\n"
                       "3) Print hex\n"
                       "4) Print string\n"
                       "5) Get string\n"
                       "6) Quit\n"
                       "Option:");


        char *char1[50];
        gets(char1);
        char i = char1[0];
        if('0' <= i && i <= '6'){
            printf("Within bounds\n");
            if(i == '0')
                arr1 = map(arr1, base_len, censor);
            if(i == '1')
                arr1 = map(arr1, base_len, encrypt);
            if(i == '2')
                arr1 = map(arr1, base_len, decrypt);
            if(i == '3')
                arr1 = map(arr1, base_len, xprt);
            if(i == '4')
                arr1 = map(arr1, base_len, cprt);
            if(i == '5')
                arr1 = map(arr1, base_len, my_get);
            if(i == '6')
                arr1 = map(arr1, base_len, quit);
            printf("DONE.\n\n");

        }
    }

}