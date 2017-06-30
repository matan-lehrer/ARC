all: clean task

task: task.o JobControl.o LineParser.o
	gcc -m32 -g -Wall -o myShell task.o JobControl.o LineParser.o 
	
task.o: task.c
	gcc -g -Wall -m32 -ansi -c -o task.o task.c
	
JobControl.o: JobControl.c
	gcc -g -Wall -m32 -ansi -c -o JobControl.o JobControl.c
	
LineParser.o: LineParser.c
	gcc -g -Wall -m32 -ansi -c -o LineParser.o LineParser.c

.PHONY: clean

clean: 
	rm -f *.o myShell
