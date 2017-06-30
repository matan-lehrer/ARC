all: clean task0

task0: task0.o LineParser.o
	gcc -m32 -g -Wall -o task0 task0.o LineParser.o
	
task0.o: task0.c
	gcc -g -Wall -m32 -ansi -c -o task0.o task0.c
	
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -ansi -c -o LineParser.o LineParser.c
	
.PHONY: clean

clean: 
	rm -f *.o task0
