#include "LineParser.h"
#include "variable.h"
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

struct var *var_list = NULL;
 

void executeDolar(cmdLine *pCmdLine) 
{
  int i;
  for(i=0; i<pCmdLine->argCount; i++)
  {
    if(strncmp("$", pCmdLine->arguments[i], 1)==0)
    {
      var *var_list_temp = locationVar(&pCmdLine->arguments[i][1],var_list);
      if(var_list_temp != NULL)
	replaceCmdArg(pCmdLine, i, var_list_temp->value);     
    }
  }
}


void isReUseFunction(char* input, int d_flag)
{
  if(input[0] == '!')
  {
    int num = atoi(&input[1]);
 
    if(num>=0 && num<=9)
    {      
      if(index<HIST_SIZE)
      {
	strcpy(input, history[num]); 	
      }
      else
      {
	strcpy(input, history[(num+index%HIST_SIZE)%HIST_SIZE]);
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
    executeDolar(pCmdLine);
    if(strcmp(pCmdLine->arguments[0], "echo") == 0){
      if (strcmp(pCmdLine->arguments[1], "~") == 0) {
	   printf("echo : %s \n", getenv("HOME"));
      }
      else {
	int argNum  = pCmdLine->argCount ; 
	for (i =1 ; i < argNum ; i++) 
		printf("echo %d : %s \n", i, pCmdLine->arguments[i]);
      }
      return 1;   
    }
  return -1; 
}

int isHistoryFunction(cmdLine *pCmdLine){
    int i;
    executeDolar(pCmdLine);
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
    executeDolar(pCmdLine);
    
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
  struct var *v;
  int d_flag = 0;
  
  v = (struct var *) malloc ( sizeof (var) );
	v->next = NULL;
	
	v->name = (char*)malloc(strlen("~"));
	strcpy(v->name, "~");
	
	v->value = (char*)malloc(strlen(getenv("HOME")));
	strcpy(v->value, getenv("HOME"));
	
	var_list = addVar(var_list, v); 
	
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
      list_free(var_list);
      break;
    }
    else 
    {
      if(strlen(input) == 1)
	continue;
      
      isReUseFunction(input,d_flag);
      
      strcpy(history[index%HIST_SIZE], input);
      index++;
      
      cmd = parseCmdLines(input);
      
      if(strcmp("set", cmd->arguments[0])==0)
      {
	v = (struct var *) malloc ( sizeof (var) );
	v->next = NULL;
	
	v->name = (char*)malloc(strlen(cmd->arguments[1]));
	strcpy(v->name, cmd->arguments[1]);
	
	v->value = (char*)malloc(strlen(cmd->arguments[2]));
	strcpy(v->value, cmd->arguments[2]);
	
	var_list = addVar(var_list, v); 	
      }
    
      else if(strcmp("delete", cmd->arguments[0])==0)
      	var_list = removeVar(cmd->arguments[1], var_list);
            
      else if(strcmp("env", cmd->arguments[0])==0)
	printEnv(var_list);  
    
      else if((strcmp("cd", cmd->arguments[0])==0)&&(strcmp("~", cmd->arguments[1])==0)){
	  var *var_list_temp = locationVar(cmd->arguments[1],var_list);
	  if(var_list_temp != NULL)
	      replaceCmdArg(cmd, 1, var_list_temp->value); 
      }

	      
      else
      {
      if(isEchoFunction(cmd) == -1 && isHistoryFunction(cmd) == -1)
	  execute(cmd,d_flag);  
      }
     
      freeCmdLines(cmd);
    }
  }
  
  return 0;
}
