all: clean task2

task2: task2.o LineParser.o variable.o
	gcc -m32 -g -Wall -o task2 task2.o LineParser.o variable.o
	
task2.o: task2.c
	gcc -g -Wall -m32 -ansi -c -o task2.o task2.c
	
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -ansi -c -o LineParser.o LineParser.c
	
variable.o: variable.c
	gcc -g -Wall -m32 -ansi -c -o variable.o variable.c
	
.PHONY: clean

clean: 
	rm -f *.o task2
