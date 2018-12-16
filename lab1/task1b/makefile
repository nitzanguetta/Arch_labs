#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: wordCounter

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
wordCounter: wordCounter.o
	gcc -g -Wall -o wordCounter wordCounter.o

# Depends on the source and header files
wordCounter.o: wordCounter.c 
	gcc -g -Wall -c -o wordCounter.o wordCounter.c 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o hello
 
