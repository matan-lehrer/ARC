#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: encoder

# Tool invocations
# Executable "encoder" depends on the files encoder.o 
encoder: encoder.o
	gcc -m32 -g -Wall -o encoder encoder.o 

# Depends on the source and header files
encoder.o: encoder.c 
	gcc -g -Wall -m32 -ansi -c -o encoder.o encoder.c

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o encoder