# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
CC = g++
CFLAGS  = -g -Wall -Weffc++ -std=c++11
LFLAGS  = -L/usr/lib

# All Targets
all: hello

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
hello: bin/HelloWorld.o bin/Run.o
	@echo 'Building target: hello'
	@echo 'Invoking: C++ Linker'
	$(CC) -o bin/hello bin/HelloWorld.o bin/Run.o $(LFLAGS)
	@echo 'Finished building target: hello'
	@echo ' '

# Depends on the source and header files
bin/hello.o: src/HelloWorld.cpp
	$(CC) $(CFLAGS) -c -Iinclude -o bin/HelloWorld.o src/HelloWorld.cpp

# Depends on the source and header files 
bin/Run.o: src/Run.cpp
	$(CC) $(CFLAGS) -c -Iinclude -o bin/Run.o src/Run.cpp

#Clean the build directory
clean: 
	rm -f bin/*