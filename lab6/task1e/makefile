#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: myshell

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
myshell: myshell.o line_parser.o job_control.o
	gcc -g -Wall -o myshell myshell.o line_parser.o job_control.o 
	
# Depends on the source and header files
myshell.o: myshell.c 
	gcc -g -Wall -c -o myshell.o myshell.c 

line_parser.o: line_parser.c
	gcc -g -Wall -c -o line_parser.o line_parser.c 
job_control.o: job_control.c
	gcc -g -Wall -c -o job_control.o job_control.c 


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o myshell
