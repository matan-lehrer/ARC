#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <wait.h>

int pipe_input[2];
int flag_debug = 0;

void child_process1() ;
void child_process2() ;
void parent_process1() ;
void parent_process2() ;
FILE *log;


int main(int argc, char ** argv,char **envp){
    log = fopen("log.txt", "w+");
   int child1, child2;

    int i;
    for(i = 1; i < argc; i++){
        if(strcmp("-d", argv[i]) == 0)
            flag_debug = 1;
    }

    assert(pipe(pipe_input) != -1);

    if(flag_debug){
        fprintf(log, "parent_process>forking…\n");
    }

    child1 = fork();

    if(flag_debug && 0 < child1){
        fprintf(log, "parent_process>created process with id: %d\n", child1);
    }

    switch (child1) {
        case -1:
            if(flag_debug){
                fprintf(log, "The call to fork() failed. Quitting!\n");
            }else
                fprintf(log, "The call to fork() failed. Quitting!\n");
            exit(-1);

        case 0:
            child_process1();
            break;

        default:
            parent_process1();
    }

    if(flag_debug){
        fprintf(log, "parent_process>forking…\n");
    }

    child2 = fork();

    if(flag_debug && 0 < child2){
        fprintf(log, "parent_process>created process with id: %d\n", child2);
    }

    switch (child2) {
        case -1:
            if(flag_debug){
                fprintf(log, "The call to fork() failed. Quitting!\n");
            }else
                fprintf(log, "The call to fork() failed. Quitting!\n");
            exit(-1);

        case 0:
            child_process2();
            break;

        default:
            parent_process2();
    }

    if(flag_debug){
        fprintf(log, "parent_process>waiting for child processes to terminate…\n");
    }

    assert(waitpid(child1, 0, 0) != -1);

    assert(waitpid(child2, 0, 0) != -1);
    if(flag_debug){
        fprintf(log, "parent_process>exiting…\n");
    }

    return 0;
}

void child_process1() {
    if(flag_debug){
        fprintf(log, "child1>redirecting stdout to the write end of the pipe…\n");
    }
    close(1);
    assert(dup2(pipe_input[1], 1) != -1);
    close(pipe_input[1]);
    char *arrLs[] = {"ls", "-1", 0};
    if(flag_debug){
        fprintf(log, "child1>going to execute cmd: %s\n", arrLs[0]);
    }
    if(execvp("ls", arrLs) == -1){
        perror((const char *) arrLs);
        _exit(0);
    }
}

void child_process2() {
    if(flag_debug){
        fprintf(log, "child2>redirecting stdout to the write end of the pipe…\n");
    }
    close(0);
    assert(dup2(pipe_input[0], 0) != -1);
    close(pipe_input[0]);
    char *arrTail[] = {"ls", "-n","2", 0};
    if(flag_debug){
        fprintf(log, "child2>going to execute cmd: %s\n", arrTail[0]);
    }
    if(execvp("tail", arrTail) == -1){
        perror((const char *) arrTail);
        _exit(0);
    }
}

void parent_process1() {
    if(flag_debug){
        fprintf(log, "parent_process>closing the write end of the pipe…\n");
    }
    close(pipe_input[1]);
}

void parent_process2() {
    if(flag_debug){
        fprintf(log, "parent_process>closing the read end of the pipe…\n");
    }
    close(pipe_input[0]);
}

