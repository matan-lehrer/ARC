#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: clean virus

# Tool invocations
# Executable "virus" depends on the files util.o and flame2.o.
virus: task3.o  task2.o  task1.o 
	ld -m elf_i386 task3.o -o virus
	ld -m elf_i386 task2.o -o virus
	ld -m elf_i386 task1.o -o virus
	cp ./clean/ELFexec ELFexec
	cp ./clean/ELFexec2long ELFexec2long

# Depends on the source and header files
	
task3.o: task3.s
	nasm -f elf32 task3.s -o task3.o
	
task2.o: task2.s
	nasm -f elf32 task2.s -o task2.o
	
task1.o: task1.s
	nasm -f elf32 task1.s -o task1.o


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o virus
	rm ELFexec
	rm ELFexec2long
	