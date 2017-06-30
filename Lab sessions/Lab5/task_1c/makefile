all: clean task1c

task1c: task1c.o LineParser.o
	gcc -m32 -g -Wall -o task1c task1c.o LineParser.o
	
task1c.o: task1c.c
	gcc -g -Wall -m32 -ansi -c -o task1c.o task1c.c
	
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -ansi -c -o LineParser.o LineParser.c
	
.PHONY: clean

clean: 
	rm -f *.o task1c
