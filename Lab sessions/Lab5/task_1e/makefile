all: clean task1e

task1e: task1e.o LineParser.o
	gcc -m32 -g -Wall -o task1e task1e.o LineParser.o
	
task1e.o: task1e.c
	gcc -g -Wall -m32 -ansi -c -o task1e.o task1e.c
	
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -ansi -c -o LineParser.o LineParser.c
	
.PHONY: clean

clean: 
	rm -f *.o task1e
