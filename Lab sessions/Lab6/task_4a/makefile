all: clean task4a

task4a: task4a.o LineParser.o
	gcc -m32 -g -Wall -o task4a task4a.o LineParser.o
	
task4a.o: task4a.c
	gcc -g -Wall -m32 -ansi -c -o task4a.o task4a.c
	
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -ansi -c -o LineParser.o LineParser.c
	
.PHONY: clean

clean: 
	rm -f *.o task4a
