#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include  <sys/types.h>

int main(int argc, char *argv[])
{
	FILE *log;
  	close(2);
  	log = fopen("log.txt", "w+");
    int pipefd[2];
    pid_t cpid1;
    pid_t cpid2;
        
    char *str1[] = {"ls","-l",0};
    char *str2[] = {"tail","-n","2",0};
	
	int d_flag = 0;
  
    if(argc > 1)
	{
		if(strcmp("-d", argv[1]) == 0) 
			d_flag = 1; 
	}
	
    if (pipe(pipefd) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
	if(d_flag) 
   {
	   fprintf(log, "(parent_process>forking..) \n");
   }
   
    cpid1 = fork();
    
	if(d_flag && cpid1 != -1) 
   {
	   fprintf(log, "(parent_process>created process with id: %d ) \n", cpid1);
   }
	
    if (cpid1 == -1) 
    {
        perror("fork failure");
        exit(EXIT_FAILURE);
    }
    
     
    if (cpid1 == 0) /* Child reads from pipe */
    {
      close(1);
	  if(d_flag) 
		{
	     fprintf(log, "(child1>redirecting stdout to the write end of the pipe..) \n");
		}    
      dup(pipefd[1]);
      close(pipefd[1]); 
	  if(d_flag) 
		{
	     fprintf(log, "(child1>going to execute cmd: ..) \n");
		}
      execvp(str1[0],str1);  
    }
    else
	{
		if(d_flag) 
	   {
		   fprintf(log, "(parent_process>closing the write end of the pipe..) \n");
	   }
		close(pipefd[1]);
	}
    
	if(d_flag) 
   {
	   fprintf(log, "(parent_process>forking..) \n");
   }
   
    cpid2 = fork();
    
    if(d_flag && cpid2 != -1) 
   {
	   fprintf(log, "(parent_process>created process with id: %d ) \n", cpid2);
   }
	
    if (cpid2 == -1) 
    {
        perror("fork failure");
        exit(EXIT_FAILURE);
    }
    
    if (cpid2 == 0) /* Child reads from pipe */
    {
      close(0);
	  if(d_flag) 
		{
	     fprintf(log, "(child2>redirecting stdin to the read end of the pipe..) \n");
		}    
      dup(pipefd[0]);
      close(pipefd[0]);
	  if(d_flag) 
		{
	     fprintf(log, "(child2>going to execute cmd: ..) \n");
		}
      execvp(str2[0],str2);  
    }
    else 
	{
		if(d_flag) 
		{
	     fprintf(log, "(parent_process>closing the read end of the pipe..) \n");
		}
        close(pipefd[0]);
	}
     if(d_flag) 
		{
	     fprintf(log, "(parent_process>waiting for child processes to terminate..) \n");
		}
    if(waitpid(cpid1,0,0) == -1) 
	{
		if(d_flag) 
		{
	     fprintf(log, "(parent_process>exiting..) \n");
		}
       _exit(EXIT_SUCCESS);
	}
    if(d_flag) 
		{
	     fprintf(log, "(parent_process>waiting for child processes to terminate..) \n");
		}
    if(waitpid(cpid2,0,0) == -1)
	{
		if(d_flag) 
		{
	     fprintf(log, "(parent_process>exiting..) \n");
		}
       _exit(EXIT_SUCCESS);
	}
	fclose(log);

    return 0;
}