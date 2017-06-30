#include "JobControl.h"
#include "LineParser.h"
#include "variable.h"
#include <assert.h>

#define INPUT_SIZE 2048

void child_process(cmdLine *pLine, job *pJob);
void parent_process(int child_pid, cmdLine *pLine, job *pJob);
void execute();

char inputLine[INPUT_SIZE];
int pipe_input[2];
job* job_list = NULL;
var* variables_list = NULL;
__pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int flag_debug = 0;
char *directoryPath, initDirectoryPath;
char *buf;
FILE *log;

int main(int argc, char ** argv, char **envp){
    log = fopen("log.txt", "w+");
    int i;
    for(i = 1; i < argc; i++){
        if(strcmp("-d", argv[i]) == 0)
            flag_debug = 1;
    }

    /*Task 2C*/
    if (flag_debug)
        fprintf(log, "Ignore signal name: %s \n", strsignal(SIGTTIN));
    signal (SIGTTIN, SIG_IGN);
    if (flag_debug)
        fprintf(log, "Ignore signal name: %s \n", strsignal(SIGTTOU));
    signal (SIGTTOU, SIG_IGN);
    if (flag_debug)
        fprintf(log, "Ignore signal name: %s \n", strsignal(SIGTSTP));
    signal (SIGTSTP, SIG_IGN);

    if (flag_debug)
        fprintf(log, "Ignore signal name: %s \n", strsignal(SIGQUIT));
    signal (SIGQUIT, SIG_IGN);
    /*if (flag_debug)
        fprintf(log, "Ignore signal name: %s \n", strsignal(SIGCHLD));
    signal (SIGCHLD, SIG_IGN);
*/
    shell_pgid = getpid ();

    if (flag_debug)
        fprintf(log, "save the shell terminal attributes \n");
    shell_terminal = STDIN_FILENO;
    tcgetattr(shell_terminal, &shell_tmodes);
    /*End of task 2C*/
    assert((initDirectoryPath = getcwd(buf, INPUT_SIZE)) != NULL);

    while(1) {
        assert((directoryPath = getcwd(buf, INPUT_SIZE)) != NULL);

        printf("%s~> ", directoryPath);

        fgets(inputLine, INPUT_SIZE, stdin);

        if (flag_debug)
            fprintf(log, "Execute command: %s ", inputLine);

        if (strcmp(inputLine, "quit\n") == 0)
            break;

        if (strlen(inputLine) == 1)
            continue;

        execute();
    }

    if (flag_debug)
        fprintf(log, "Print and free Job list \n");
    printJobs(&job_list);
    freeJobList(&job_list);

    return 0;
}

void execute() {
    cmdLine *pCmdLine = 0;
    int pid;

    if(strcmp(inputLine, "jobs\n") == 0){
        if(flag_debug){
            fprintf(log, "going to execute cmd: jobs");
        }
        printJobs(&job_list);
        return;
    }

    job * newjob = addJob(&job_list, inputLine);
    newjob->cmd = strdup(inputLine);
    pCmdLine = parseCmdLines(inputLine);

    if(strcmp(pCmdLine->arguments[0], "cd") == 0) {
        if (flag_debug) {
            fprintf(log, "going to execute cmd: %s\n", pCmdLine->arguments[0]);
        }
        if (strcmp(pCmdLine->arguments[1], "~") == 0) {
            assert((chdir(initDirectoryPath) != -1));
        }else
            assert((chdir(pCmdLine->arguments[1]) != -1));

        return;
    }

    if(strcmp(pCmdLine->arguments[0], "fg") == 0){
        if(flag_debug){
            fprintf(log, "going to execute cmd: %s\n", pCmdLine->arguments[0]);
        }
        int cont = atoi(pCmdLine->arguments[1]);
        job* jobFg = findJobByIndex(job_list, cont);

        runJobInForeground(&job_list, jobFg ,
                           1, &shell_tmodes, shell_pgid);
        return;
    }

    if(strcmp(pCmdLine->arguments[0], "bg") == 0){
        if(flag_debug){
            fprintf(log, "going to execute cmd: %s\n", pCmdLine->arguments[0]);
        }
        int cont = atoi(pCmdLine->arguments[1]);
        job* jobBg = findJobByIndex(job_list, cont);
        jobBg->status = RUNNING;
        runJobInBackground(jobBg, 1);
        return;
    }

    /*The change for task 4*/
    if(strcmp(pCmdLine->arguments[0], "set") == 0){
        if(pCmdLine->arguments[1] != NULL && pCmdLine->arguments[2] != NULL)
            addVar(&variables_list, pCmdLine->arguments[1], pCmdLine->arguments[2]);
        else if(flag_debug){
            fprintf(log, "Too fwe arguments");
        }
        return;
    }

    if(strcmp(pCmdLine->arguments[0], "env") == 0){
        printVar(&variables_list);
        return;
    }

    if(strcmp(pCmdLine->arguments[0], "delete") == 0){
        removeVar(&variables_list, pCmdLine->arguments[1]);
        return;
    }

    int i;
    for(i=0; i<pCmdLine->argCount; i++) {
        if(strncmp("$", pCmdLine->arguments[i], 1)==0) {
            replaceCmdArg(pCmdLine, i, findValueByName(&variables_list, &pCmdLine->arguments[i][1]));
        }
    }

    assert(pipe(pipe_input) != -1);
    /*end of change*/

    while (pCmdLine != NULL) {
        if (flag_debug) {
            fprintf(log, "parent_process>forking…\n");
        }
        pid = fork();
        if (flag_debug /*&& 0 < pid*/) {
            fprintf(log, "parent_process>created process with id: %d\n", pid);
        }

        switch (pid) {
            case -1:
                fprintf(log, "The call to fork() failed. Quitting!\n");
                exit(-1);

            case 0:
                child_process(pCmdLine, newjob);
                break;

            default:
                parent_process(pid, pCmdLine, newjob);
        }

        pCmdLine = pCmdLine->next;
    }

    if (flag_debug)
        fprintf(log, "Free Cmd Lines: %s", inputLine);
    freeCmdLines(pCmdLine);
}

void child_process(cmdLine *pCmdLine, job *pJob) {
    /*Task 2C*/
    if (flag_debug)
        fprintf(log, "child_process>Set the signal handlers back to default: %s \n", strsignal(SIGTTIN));
    signal (SIGTTIN, SIG_DFL);
    if (flag_debug)
        fprintf(log, "child_process>Set the signal handlers back to default: %s \n", strsignal(SIGTTOU));
    signal (SIGTTOU, SIG_DFL);
    if (flag_debug)
        fprintf(log, "child_process>Set the signal handlers back to default: %s \n", strsignal(SIGTSTP));
    signal (SIGTSTP, SIG_DFL);
    if (flag_debug)
        fprintf(log, "child_process>Set the signal handlers back to default: %s \n", strsignal(SIGQUIT));
    signal (SIGQUIT, SIG_DFL);
    if (flag_debug)
        fprintf(log, "child_process>Set the signal handlers back to default: %s \n", strsignal(SIGCHLD));
    signal (SIGCHLD, SIG_DFL);

    int child_pid = getpid();

    if (flag_debug)
        fprintf(log, "child_process>Set the process group id %d \n", child_pid);
    setpgid(child_pid, child_pid);

    pJob->pgid = child_pid;
    /*End of task 2C*/

    /*The change for task 3*/
    if(pCmdLine->next != NULL){
        if(flag_debug){
            fprintf(log, "child1>redirecting stdout to the write end of the pipe…\n");
        }
        close(1);
        assert(dup2(pipe_input[1], 1) != -1);
        close(pipe_input[1]);
    }else{
        if(flag_debug){
            fprintf(log, "child2>redirecting stdin to the read end of the pipe…\n");
        }
        close(0);
        assert(dup2(pipe_input[0], 0) != -1);
        close(pipe_input[0]);
    }
    /*end of change*/

    /*Task 1*/
    if(pCmdLine->inputRedirect != NULL) {
        close(0);
        fopen(pCmdLine->inputRedirect, "r");
    }

    if(pCmdLine->outputRedirect != NULL) {
        close(1);
        fopen(pCmdLine->outputRedirect, "w");
    }
    /*Task 1*/

    if(execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1){
        perror(pCmdLine->arguments[0]);
        _exit(0);
    }
}

void parent_process(int child_pid, cmdLine *pCmdLine, job *pJob) {
    /*Task 2C*/
    if (flag_debug)
        fprintf(log, "parent_process>Set the process group id %d \n", child_pid);
    setpgid(child_pid, child_pid);

    pJob->pgid = child_pid;
    /*End of task 2C*/

    /*Task 1*/
    if(pCmdLine->next != NULL){
        if(flag_debug){
            fprintf(log, "parent_process>closing the write end of the pipe…\n");
        }
        close(pipe_input[1]);
    }else{
        if(flag_debug){
            fprintf(log, "parent_process>closing the read end of the pipe…\n");
        }
        close(pipe_input[0]);
    }
    /*end of task 1*/

    if(pCmdLine->blocking == 1) {
        if (flag_debug) {
            fprintf(log, "parent_process>waiting for child processes to terminate…\n");
        }
        assert(waitpid(child_pid, 0, 0) != -1);
    }
    if(flag_debug){
        fprintf(log, "parent_process>exiting…\n");
    }
}

