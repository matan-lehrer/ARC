#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: calc

# Tool invocations
# Executable "maincalc" depends on the 
calc: ass2.o
	gcc -m32 -Wall -g calc.o -o calc

# Depends on the source
ass2.o:	 
	nasm -f elf calc.s -o calc.o


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o calc