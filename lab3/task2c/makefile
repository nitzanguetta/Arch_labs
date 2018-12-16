#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: main

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
main: main[1].o toy_printf.o
	gcc -g -Wall -o main main[1].o toy_printf.o 
	
# Depends on the source and header files
main[1].o: main[1].c 
	gcc -g -Wall -c -o main[1].o main[1].c 

toy_printf.o: toy_printf.c
	gcc -g -Wall -c -o toy_printf.o toy_printf.c 


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o main
 
