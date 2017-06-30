all: run

run: task4.o
	gcc -g -m32 -Wall -o run task4.o

task4.o: task4.c
	gcc -g -m32 -ansi -Wall -c -o task4.o task4.c

.PHONY: clean

clean:
	rm -f *.o run