all: clean task1

task1: task1.o LineParser.o
	gcc -m32 -g -Wall -o task1 task1.o LineParser.o
	
task1.o: task1.c
	gcc -g -Wall -m32 -ansi -c -o task1.o task1.c
	
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -ansi -c -o LineParser.o LineParser.c
	
.PHONY: clean

clean: 
	rm -f *.o task1
