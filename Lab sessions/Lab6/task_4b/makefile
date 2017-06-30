all: clean task4b

task4b: task4b.o LineParser.o
	gcc -m32 -g -Wall -o task4b task4b.o LineParser.o
	
task4b.o: task4b.c
	gcc -g -Wall -m32 -ansi -c -o task4b.o task4b.c
	
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -ansi -c -o LineParser.o LineParser.c
	
.PHONY: clean

clean: 
	rm -f *.o task4b
