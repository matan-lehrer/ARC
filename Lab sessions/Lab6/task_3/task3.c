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
    int first_pipefd[2];
    int secound_pipefd[2];
    pid_t cpid1;
    pid_t cpid2;
    pid_t cpid3;
        
    char *str1[] = {"ls","-l",0};
    char *str2[] = {"tail","-n","2",0};
    char *str3[] = {"wc","-l",0};

	
    int d_flag = 0;
    int exit_suc = 0; 
  
    if(argc > 1)
	{
		if(strcmp("-d", argv[1]) == 0) 
			d_flag = 1; 
	}
	
    if (pipe(first_pipefd) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
	if (pipe(secound_pipefd) == -1) 
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
      dup(first_pipefd[1]);
      close(first_pipefd[1]); 
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
		close(first_pipefd[1]);
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
      dup(first_pipefd[0]);
	  if(d_flag) 
		{
	     fprintf(log, "(parent_process>closing the read end of the pipe..) \n");
		}
      close(first_pipefd[0]);
	  close(1);
	  if(d_flag) 
		{
	     fprintf(log, "(child2>redirecting stdout to the write end of the pipe..) \n");
		}    
      dup(secound_pipefd[1]);
	  if(d_flag) 
		{
	     fprintf(log, "(parent_process>closing the read end of the pipe..) \n");
		}
      close(secound_pipefd[1]);
	  
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
        close(secound_pipefd[1]);
	}
	
		if(d_flag) 
   {
	   fprintf(log, "(parent_process>forking..) \n");
   }
   
    cpid3 = fork();
    
	if(d_flag && cpid1 != -1) 
   {
	   fprintf(log, "(parent_process>created process with id: %d ) \n", cpid3);
   }
	
    if (cpid3 == -1) 
    {
        perror("fork failure");
        exit(EXIT_FAILURE);
    }
    
     
    if (cpid3 == 0) /* Child reads from pipe */
    {
      close(0);
	  if(d_flag) 
		{
	     fprintf(log, "(child3>redirecting stdout to the write end of the pipe..) \n");
		}    
      dup(secound_pipefd[0]);
      close(secound_pipefd[0]); 
	  if(d_flag) 
		{
	     fprintf(log, "(child3>going to execute cmd: ..) \n");
		}
      execvp(str3[0],str3);  
    }
    else
	{
		if(d_flag) 
	   {
		   fprintf(log, "(parent_process>closing the write end of the pipe..) \n");
	   }
		close(secound_pipefd[0]);
	}
	
	
	
     if(d_flag) 
	{
	     fprintf(log, "(1-parent_process>waiting for child processes to terminate..) \n");
	}
    if(waitpid(cpid1,0,0) == -1) 
	{
	  exit_suc = 1 ; 
           _exit(EXIT_SUCCESS);
	}
	
    if(d_flag) 
	{
	     fprintf(log, "(2-parent_process>waiting for child processes to terminate..) \n");
	}
    if(waitpid(cpid2,0,0) == -1)
	{
	   exit_suc = 1; 
	   _exit(EXIT_SUCCESS);
	}
	
    if(d_flag) 
	{
	     fprintf(log, "(3-parent_process>waiting for child processes to terminate..) \n");
	}
    if(waitpid(cpid3,0,0) == -1) 
	{
	   exit_suc = 1; 
           _exit(EXIT_SUCCESS);
	}
    if(d_flag && exit_suc == 0) 
	{
	     fprintf(log, "(parent_process>exiting..) \n");
	}
	fclose(log);
    return 0;
}