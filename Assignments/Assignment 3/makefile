#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: ass3

# Tool invocations
# Executable "flame2" depends on the files util.o and flame2.o.
ass3: coroutines.o cell.o scheduler.o printer.o ass3.o
	gcc -g -m32 -Wall -o ass3 coroutines.o cell.o scheduler.o printer.o ass3.o

# Depends on the source and header files
	
cell.o: cell.c
	gcc -m32 -Wall -ansi -c cell.c -o cell.o
	
coroutines.o: coroutines.s
	nasm -f elf coroutines.s -o coroutines.o
	
scheduler.o: scheduler.s
	nasm -f elf scheduler.s -o scheduler.o
	
printer.o: printer.s
	nasm -f elf printer.s -o printer.o
	
ass3.o: ass3.s
	nasm -f elf ass3.s -o ass3.o


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o ass3
	