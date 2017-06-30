#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

char encrypt(char c) { /* Gets a char c and returns its encrypted form by adding 3 to its value.
          If c is not between 0x20 and 0x7E it is returned unchanged */
    if(32 <= c && c <= 176){
        return c;
    }
    return c + 3;
}
char decrypt(char c) { /* Gets a char c and returns its decrypted form by reducing 3 to its value.
            If c is not between 0x20 and 0x7E it is returned unchanged */
    if(32 <= c && c <= 176){
        return c;
    }
    return c - 3;
}
char xprt(char c) { /* xprt prints the value of c in a hexadecimal representation followed by a
           new line, and returns c unchanged. */
    printf("0x%x\n", c);
    return c;

}
char cprt(char c) { /* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line.
                    Otherwise, cprt prints the dot ('.') character. After printing, cprt returns the value of c unchanged. */
    if(32 <= c && c <= 176){
        printf("%c\n", c);
    }else
        printf(".\n");

    return c ;

}
char my_get(char c){  /* Ignores c, reads and returns a character from stdin using fgetc. */

    return fgetc(stdin);
}

char quit(char c){
    exit(c);
}
char censor(char c) {
    if(c == '!')
        return '.';
    else
        return c;
}

char* map(char *array, int array_length, char (*f) (char)){
    long int i = 0;
    for (i = 0; i < array_length; i++)
        array[i] = f(array[i]);
    return array;
}

int main(int argc, char **argv){
    int base_len = 5;
    char* mapped_array = (char*)(malloc(base_len*sizeof(char)));
    char* arr2 = map(mapped_array, base_len, my_get);
    char* arr3 = map(arr2, base_len, encrypt);
    char* arr4 = map(arr3, base_len, xprt);
    char* arr5 = map(arr4, base_len, decrypt);
    char* arr6 = map(arr5, base_len, cprt);

    /*
    printf("%s\n", mapped_array);

    printf("%s\n", arr2);
    printf("%s\n", arr3);
    printf("%s\n", arr4);
    printf("%s\n", arr5);
    printf("%s\n", arr6);
*/
    free(arr2);

}