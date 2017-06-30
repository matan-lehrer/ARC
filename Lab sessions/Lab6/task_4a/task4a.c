#include "LineParser.h"
#include <stdio.h>
#include <stdlib.h>
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

/*This function receives the number of required pipes and returns an array of pipes*/
int **createPipes(int nPipes)
{
  int **arr_pipes = malloc (nPipes*sizeof(int *));
  
  int i;
  for(i=0; i<nPipes; i++)
  {
    int *pipefd = malloc (2*sizeof(int));
    if (pipe(pipefd) == -1) 
    {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
    arr_pipes[i] = pipefd;
  }
  return arr_pipes;
}

/*This function receives an array of pipes and an integer indicating the size of the array. The function releases all memory dedicated to the pipes*/
void releasePipes(int **pipes, int nPipes)
{
  int i;
  for(i=0; i<nPipes; i++)
  {
    free(pipes[i]);
  }
  free(pipes);
}

/*This function receives an array of pipes and a pointer to a cmdLine structure. It returns the pipe which feeds the process associated with the command. 
 That is, the pipe that appears to the left of the process name in the command line*/
int *leftPipe(int **pipes, cmdLine *pCmdLine)
{
  int index = pCmdLine->idx;
  if(index > 0)
    return pipes[index-1];
  else
    return NULL;
}

/*This function receives an array of pipes and a pointer to a cmdLine structure. It returns the pipe which is the sink of the associated command. 
 *That is, the pipe that appears to the right of the process name in the command line*/
int *rightPipe(int **pipes, cmdLine *pCmdLine)
{
  int index = pCmdLine->idx;
  if(pCmdLine->next != NULL)
    return pipes[index];
  else
    return NULL;
}

void isReUseFunction(char* input,int d_flag)
{
  if(input[0] == '!')
  {
    int num = atoi(&input[1]);
    int min = ( index < HIST_SIZE ) ? index : HIST_SIZE;

    if(num>=0 && num<=9)
    {      
      if(index<HIST_SIZE)
      {
	strcpy(input, history[min-num-1]); 	
      }
      else
      {
	strcpy(input, history[min-((num+index%HIST_SIZE)%HIST_SIZE)-1]);
      }      
    }
    else{
      if(d_flag)
	     fprintf(stderr, "you want to do an action that not yet been made\n");
	  else
		printf("error\n");    
	}
  } 
}

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

/*int main(int argc, char ** argv,char **envp) 
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
    printf("Current working directory : %s >",buffer);
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
      
      isReUseFunction(input, d_flag);
      
      strcpy(history[index%HIST_SIZE], input);
      index++;
      
      cmd = parseCmdLines(input);
      
      if(isEchoFunction(cmd) == -1 && isHistoryFunction(cmd) == -1)
	  execute(cmd, d_flag);  
     
      freeCmdLines(cmd);
    }
  }
  
  return 0;
}*/

int main(int argc, char ** argv,char **envp) 
{
  cmdLine *PCmdLine = parseCmdLines("ls | tee | cat | more");
  int **pipes = createPipes(3);
  if(leftPipe(pipes, PCmdLine) == NULL){
    puts("the left pipe of ls is null!");
  }
  if(rightPipe(pipes, PCmdLine->next->next->next) == NULL){
    puts("the right pipe of more is null!");
  }
  printf("tee leftPipe[0]: %d\ntee leftPipe[1]: %d\n", leftPipe(pipes, PCmdLine->next)[0], leftPipe(pipes, PCmdLine->next)[1]);
  printf("tee rightPipe[0]: %d\ntee rightPipe[1]: %d\n", rightPipe(pipes, PCmdLine->next)[0], rightPipe(pipes, PCmdLine->next)[1]);
  releasePipes(pipes, 3);
  freeCmdLines(PCmdLine);
  return 0;
} 
