#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include "line_parser.h"
#include <string.h>

#define PATH_MAX        4096
#define CHILD           0
#define ERROR           -1
#define ERROR_EXIT      0x55

int IS_QUIT(char* input) {return !strcmp("quit\n",input);}
int IN_REDIRECT(cmd_line* line){return line->input_redirect!=NULL && line->output_redirect==NULL;}
int OUT_REDIRECT(cmd_line* line){return line->output_redirect!=NULL && line->input_redirect==NULL;}
int IN_AND_OUT_REDIRECT(cmd_line* line){return line->input_redirect!=NULL && line->output_redirect!=NULL;}
int IS_NOT_NULL(cmd_line* line){return line!=NULL;}

void execute_execvp(cmd_line *line){
    if (execvp(line->arguments[0], line->arguments) == ERROR ) {
        perror("Error: ");
        _exit(ERROR_EXIT);
    }
}

void in_redirect_exec(cmd_line *line){
    fclose(stdin);
    FILE * stream = fopen(line->input_redirect,"r");
    (!stream) ? perror("Error: CANNOT READ FILE"):
                execute_execvp(line);
    fclose(stream);
}
void out_redirect_exec(cmd_line *line){
    fclose(stdout);
    FILE * stream = fopen(line->output_redirect,"w");
    (!stream) ? perror("Error: CANNOT OPEN FILE"):
                execute_execvp(line);
    fclose(stream);
}
void in_and_out_redirect_exec(cmd_line *line){
    fclose(stdin);
    fclose(stdout);
    FILE * stream_in = fopen(line->input_redirect,"r");
    FILE * stream_out = fopen(line->output_redirect,"w");
    (!stream_in) ?  perror("Error: CANNOT READ FILE"):
    (!stream_out) ? perror("Error: CANNOT OPEN FILE"):
                    execute_execvp(line);
    fclose(stream_in);
    fclose(stream_out);
}

void no_redirect_exec(cmd_line *line){execute_execvp(line);}

void execute(cmd_line *line){
    IN_REDIRECT(line)?          in_redirect_exec(line):
    OUT_REDIRECT(line)?         out_redirect_exec(line):
    IN_AND_OUT_REDIRECT(line)?  in_and_out_redirect_exec(line):
                                no_redirect_exec(line);
}


void print_curr_dir(){
    char buffer_pwd[PATH_MAX];
    printf("%s>", getcwd(buffer_pwd, PATH_MAX));
}


int main (int argc, char *argv[])
{
    char input[PATH_MAX];
    cmd_line *cmd_parse;
    int status;

    while(1) {
        print_curr_dir();
        fgets(input, PATH_MAX, stdin);

        if(IS_QUIT(input)) break;

        cmd_parse = parse_cmd_lines(input);
        if (IS_NOT_NULL(cmd_parse)) {
                pid_t pid = fork();
                (pid==ERROR) ? perror("Error: ") : 0;

                (pid==CHILD)? execute(cmd_parse) :
                (cmd_parse->blocking)? waitpid(pid, &status, 0) : 0;

        }
        free_cmd_lines(cmd_parse);
    }
    return 0;
}
