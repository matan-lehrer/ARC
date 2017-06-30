#include <stdio.h>
#include <string.h>
#include <malloc.h>


/* return string "word" if the count is 1 or "words" otherwise */
char *words(int count) {
  char *words  = (char*) malloc(sizeof(char)*10);
  if(words == NULL)
  {
    return NULL;
  }
  strcpy(words, "words");
  if(count==1) {
    int size = strlen(words);
    words[size - 1] = '\0';
  }
  return words;
}

/* print a message reportint the number of words */
int print_word_count(char **argv) {
  int count = 0;
  char **a = argv;
  while(*(a++))
    ++count;
  printf("The sentence contains %d %s.\n", count, words(count));
  return count;
}

/* print the number of words in the command line and return the number as the exit code */
int main(int argc, char **argv) {
  return print_word_count(argv+1);
}

