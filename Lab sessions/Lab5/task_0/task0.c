#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "LineParser.h"
#include <sys/types.h>
#define PATH_MAX 4096

void freeCmdLines(cmdLine *pCmdLine);


void execute(cmdLine *pCmdLine);

cmdLine *parseCmdLines(const char *strLine);




void execute(cmdLine *pCmdLine) {
  execvp(pCmdLine -> arguments[0], pCmdLine -> arguments);
  /*
  perror("execute failed");
    _exit(1);
    
  */
}

int main(int argc , char* argv[]) {
    char inputLine[2048];
  
    char* workingDire;
    
    char buff[PATH_MAX];
    
    while(1) {
      workingDire = getcwd(buff, PATH_MAX);
      if(workingDire != NULL){
		  printf( "Current working directory is %s.\n", workingDire);
		  }
      else perror("Error getting work directory \n");
      
      fgets(inputLine, 2048, stdin);
      printf("Execute command: %s \n", inputLine);
      
      cmdLine* postParse = parseCmdLines(inputLine);
      if(postParse == NULL){
		  printf("nothing to be parse");
		  }
	  else execute(postParse);
	
      if(memcmp("quit", inputLine, 4) == 0) {
	if(postParse != NULL) {
		freeCmdLines(postParse);
		}
	break;
      }
    }

    return EXIT_SUCCESS;
}

