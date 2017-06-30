all: clean task3

task3: task3.o 
	gcc -m32 -g -Wall -o task3 task3.o 
	
task3.o: task3.c
	gcc -g -Wall -m32 -ansi -c -o task3.o task3.c
	
.PHONY: clean

clean: 
	rm -f *.o task3