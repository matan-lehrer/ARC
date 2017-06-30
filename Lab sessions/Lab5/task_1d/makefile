all: clean task1d

task1d: task1d.o LineParser.o
	gcc -m32 -g -Wall -o task1d task1d.o LineParser.o
	
task1d.o: task1d.c
	gcc -g -Wall -m32 -ansi -c -o task1d.o task1d.c
	
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -ansi -c -o LineParser.o LineParser.c
	
.PHONY: clean

clean: 
	rm -f *.o task1d
