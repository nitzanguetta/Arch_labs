#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: server

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
server: server.o line_parser.o common.o
	gcc -g -Wall -o server server.o line_parser.o common.o
	
# Depends on the source and header files
server.o: server.c 
	gcc -g -Wall -c -o server.o server.c 

line_parser.o: line_parser.c
	gcc -g -Wall -c -o line_parser.o line_parser.c 

common.o: common.c
	gcc -g -Wall -c -o common.o common.c 


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o server