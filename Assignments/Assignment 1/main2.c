#include <stdio.h>
#include <stdlib.h>
#define	BUFFER_SIZE	(128)

extern int calc_div(int x, int k);

int check(int x, int k) {
    if (x<0) return 0;
    if (k<=0 || k>31) return 0;
    return 1;
}

int main(int argc, char** argv)
{
    char buf[BUFFER_SIZE];
    fflush(stdout);
    fgets(buf, BUFFER_SIZE, stdin);
    int x = atoi(buf);
    fflush(stdout);
    fgets(buf, BUFFER_SIZE, stdin);
    int k = atoi(buf);
    calc_div(x, k);
    return 0;
}


