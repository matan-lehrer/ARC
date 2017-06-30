#include "LineParser.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include  <errno.h>

#define PATH_MAX 1000
#define INPUT_SIZE 2048
#define HIST_SIZE 10

char history[HIST_SIZE][INPUT_SIZE];
int index = 0;


int isEchoFunction(cmdLine *pCmdLine) {
    int i=0;
    if(strcmp(pCmdLine->arguments[0], "echo") == 0){
    int argNum  = pCmdLine->argCount ; 
    for (i =1 ; i < argNum ; i++) 
	    printf("echo %d : %s \n", i, pCmdLine->arguments[i]);
    return 1;   
    }
  return -1; 
}

int isHistoryFunction(cmdLine *pCmdLine){
    int i;
    if(strcmp(pCmdLine->arguments[0], "history") == 0)
    {
      int min = ( index < HIST_SIZE ) ? index : HIST_SIZE;
    
      for(i=0; i< min; i++)
      {
	if ( index < HIST_SIZE )
	    printf("%s", history[i]);
	else
	    printf("%s", history[(i+index%HIST_SIZE)%HIST_SIZE]);
      }
    return 1;
  }  
  return -1;
}

void execute(cmdLine *pCmdLine, int d_flag) 
{
  pid_t child_pid;
  child_pid = fork();
   if(d_flag) 
   {
	   fprintf(stderr, "ID: %d \n", child_pid);
   }
  if(child_pid == 0)
  {
    if(execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1) 
    {
      perror(pCmdLine->arguments[0]);
      _exit(0);
    }    
  }
  else
  {
    if(pCmdLine->blocking == 1)
    {
      int status;
      waitpid(child_pid, &status, 0);
    }
  }
}

int main(int argc, char ** argv,char **envp) 
{
  char buffer[PATH_MAX];
  char input[INPUT_SIZE];
  cmdLine *cmd = 0; 
  int d_flag = 0;
  
    if(argc > 1)
	{
		if(strcmp("-d", argv[1]) == 0) 
			d_flag = 1; 
	}
	
  while(1) 
  {
    getcwd(buffer,PATH_MAX);
    printf("Current working directory : %s>",buffer);
    fgets(input,INPUT_SIZE,stdin);
    
	if(d_flag) 
	{
		fprintf(stderr, "Execute command: %s \n", input);
	}

    if(strcmp(input,"quit\n")==0)
    {
      break;
    }
    else 
    {
      if(strlen(input) == 1)
	continue;
      
      cmd = parseCmdLines(input);
      
      strcpy(history[index%HIST_SIZE], input);
      index++;
      
      if(isEchoFunction(cmd) == -1 && isHistoryFunction(cmd) == -1)
		execute(cmd, d_flag);  
     
      freeCmdLines(cmd);
    }
  }
  
  return 0;
}
