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
  if(0x20 <= c && c <= 0x7E){
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
//    char arr1[] = {'H','E','Y','!', 0};
//     printf("%s\n", arr2)wg; /* HEY. */
//    free(arr2);

  int base_len = 5;
  char arr1[base_len];
  char* arr2 = map(arr1, base_len, my_get);
  char* arr3 = map(arr2, base_len, encrypt);
  char* arr4 = map(arr3, base_len, xprt);
  char* arr5 = map(arr4, base_len, decrypt);
  char* arr6 = map(arr5, base_len, cprt);

/*
    printf("%s\n", arr1);
    printf("%s\n", arr2);
    printf("%s\n", arr3);
    printf("%s\n", arr4);
    printf("%s\n", arr5);
    printf("%s\n", arr6);*/

  free(arr2);
  free(arr3);
  free(arr4);
  free(arr5);
  free(arr6);

}