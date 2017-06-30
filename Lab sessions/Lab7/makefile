#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: hexeditplus counter

# Tool invocations
# Executable "hexeditplus" depends on the file task1.o.
hexeditplus: task1.o
	gcc -g -m32 -Wall -o hexeditplus task1.o -lm

# Depends on the source and header files
task1.o: task1.c
	gcc -g -Wall -m32 -ansi -c -o task1.o task1.c
	
# Tool invocations
# Executable "counter" depends on the file task4.o.
counter: task4.o
	gcc -g -m32 -Wall -o counter task4.o -lm

# Depends on the source and header files
task4.o: task4.c
	gcc -g -Wall -m32 -ansi -c -o task4.o task4.c


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o hexeditplus
	rm -f *.o counter
	