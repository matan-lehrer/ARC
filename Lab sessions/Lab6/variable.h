#include <sys/types.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct var
{
    char *name;                 /* the name of the variable */
    char *value;				/* the value of the variable */
    struct var *next;	        /* next variable in chain */
} var;

void addVar(var** var_list, char *name, char *value);
void removeVar(var** var_list, const char* name);
void printVar(var** var_list);
void freeVarList(var** var_list);
char* findValueByName(var ** var_list, const char* name);
int replaceValueByName(var ** var_list, const char* name, const char* value);
