#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
 
int digit_cnt(char* argv);

int main(int argc, char** argv){
	char  *input = &argv[1][0];
	int count = digit_cnt(input);
	printf("The number of digits is: %d\n", count);
	return 0;
}

int digit_cnt(char* argv){
    int i=0, count = 0 , number_to_check;
    while (argv[i]!='\0'){
	  number_to_check = argv[i] - '0';
	  if(number_to_check>=0 && number_to_check<=9){
		count++;
	  }
	  i=i+1;  
    }
    return count;
}	
