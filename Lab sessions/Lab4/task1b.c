#include "util.h"

#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_CLOSE    6
#define SYS_LSEEK   19

#define STDIN       0
#define STDOUT      1
#define STDERR      2

#define O_RDONLY    0
#define O_WRONLY    1
#define O_RDRW      2
#define O_APPEND    1024
#define O_TRUNC     512
#define O_CREAT     64

#define SEEK_SEET   0
#define SEEK_CUR    1
#define SEEK_END    2

#define	BUFFER_SIZE	(128)

void myPrintNumber(int number);
void myPrintStr(char* str);
void myPrintStrln(char* str);

char* inputFileName = "null";
char* outputFileName = "null";
int debag = 0;

int main (int argc , char* argv[], char* envp[]){
    char buf[BUFFER_SIZE];
    int outputfile;
    int inputfile;
    int size;

    int i;
    for (i = 0 ; i < argc ; i++){
        if(strcmp(argv[i], "-i")==0){            
            inputFileName = argv[++i];
        }else if(strcmp(argv[i], "-o")==0){
            outputFileName = argv[++i];
        }else if(strcmp(argv[i], "-d")==0){
            debag = 1;;
        }
    }
    
    if(strcmp(inputFileName, "null")!=0){
         inputfile = system_call(SYS_OPEN, inputFileName, O_RDONLY, 0777);
         if(debag){
             printd(5, inputfile);
         }
         
        if(inputfile < 0){
            myPrintStrln("Couldn't read from file");
            return system_call(SYS_CLOSE, 55, 0, 0);

        }
    }
    if(strcmp(outputFileName, "null")!=0){
        outputfile = system_call(SYS_OPEN, outputFileName, O_CREAT, 0777);
        outputfile = system_call(SYS_OPEN, outputFileName, O_WRONLY, 0777);
        if(debag){
            printd(5, outputfile);

        }
        if(outputfile < 0){
            myPrintStrln("Couldn't write to file");
             return system_call(SYS_CLOSE, 55, 0, 0);
        }
    }
        
      if(strcmp(inputFileName, "null")==0){
        while(1){
            size = system_call(SYS_READ,STDIN, buf,1);
            if(debag){
            printd(3, size);
            }
            if(strcmp(buf, "\n")==0){
                if(strcmp(outputFileName, "null")==0){             
                    myPrintStr(buf);

                }else{
                    size = system_call(SYS_WRITE,outputfile, buf,strlen(buf));
                if(debag){
                    printd(4, size);
                }
                }
            break;
            }
            if(0<=strcmp(buf, "A") && strcmp(buf, "Z")<=0)
                buf[0] += 'a' - 'A';
            if(strcmp(outputFileName, "null")==0){               
                myPrintStr(buf);
            }else{
                size = system_call(SYS_WRITE,outputfile, buf,strlen(buf));
                if(debag){
                    printd(4, size);
                }
            }    
        
        }
    }else{
            while(1){
            system_call(SYS_READ, inputfile, buf, 1);
            
            if(strcmp(buf, "\n")==0){
                if(strcmp(outputFileName, "null")==0){             
                    myPrintStr(buf);
                }else{
                    size = system_call(SYS_WRITE,outputfile, buf,strlen(buf));
                    if(debag){
                        printd(4, size);
                    }
                }
                break;
            }
            if(0<=strcmp(buf, "A") && strcmp(buf, "Z")<=0)
                buf[0] += 'a' - 'A';
            if(strcmp(outputFileName, "null")==0){               
                myPrintStr(buf);
            }else{
                size = system_call(SYS_WRITE,outputfile, buf,strlen(buf));
                if(debag){
                    printd(4, size);
                }
            }    
        }
      }
    return 0;
}

void myPrintStr(char* str){
        int size;
size = system_call(SYS_WRITE,STDOUT, str,strlen(str));
    if(debag){
        printd(4, size);
    }
}

void myPrintStrln(char* str){
        int size;
size = system_call(SYS_WRITE,STDOUT, str,strlen(str));
    system_call(SYS_WRITE,STDOUT,"\n",1);
    
    if(debag){
        printd(4, size + 1);
    }
}

void myPrintNumber(int number){
    char * str = itoa(number);
        int size;

    size = system_call(SYS_WRITE,STDOUT, str,strlen(str));

    system_call(SYS_WRITE,STDOUT,"\n",1);
    
     if(debag){
        printd(4, size + 1);
    }
}


void myPrintStrlnd(char* str){
    system_call(SYS_WRITE,STDERR, str,strlen(str));
    system_call(SYS_WRITE,STDERR,"\n",1);
}

void myPrintNumberd(int number){
    char * str = itoa(number);
    
    system_call(SYS_WRITE,STDERR, str,strlen(str));

    system_call(SYS_WRITE,STDERR,"\n",1);
}

int printd (int id, int ret) {
        myPrintStrlnd("\nSystm_c id: ");
        myPrintNumberd(id);
        myPrintStrlnd("return code: ");        
        myPrintNumberd(ret);
        
}
