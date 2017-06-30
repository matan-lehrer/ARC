all: clean task1a

task1a: task1a.o LineParser.o
	gcc -m32 -g -Wall -o task1a task1a.o LineParser.o
	
task1a.o: task1a.c
	gcc -g -Wall -m32 -ansi -c -o task1a.o task1a.c
	
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -ansi -c -o LineParser.o LineParser.c
	
.PHONY: clean

clean: 
	rm -f *.o task1a
