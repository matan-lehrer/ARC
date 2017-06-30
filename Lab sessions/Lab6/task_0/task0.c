#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include  <sys/types.h>

int main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf;
    
    if (pipe(pipefd) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    cpid = fork();
    
    if (cpid == -1) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (cpid == 0) /* Child reads from pipe */
    {    
	close(pipefd[1]);          /* Close unused write end */
        
	while (read(pipefd[0], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);
        	
        close(pipefd[0]);
        _exit(EXIT_SUCCESS);
    }
    
    else  /* Parent writes "hello" to pipe */
    {
      close(pipefd[0]);          /* Close unused read end */
         
      write(pipefd[1], "hello\n", 6);
      
      close(pipefd[1]);          /* Reader will see EOF */
      wait(NULL);                /* Wait for child */
      exit(EXIT_SUCCESS);
    }
    
    return 0;
}