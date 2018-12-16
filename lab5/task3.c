#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include "line_parser.h"
#include <string.h>

#define PATH_MAX        4096
#define CHILD           0
#define ERROR           -1
#define ERROR_EXIT           -1

int IS_QUIT(char* input) {return !strcmp("quit\n",input);}
int IN_REDIRECT(cmd_line* line){return line->input_redirect!=NULL && line->output_redirect==NULL;}
int OUT_REDIRECT(cmd_line* line){return line->output_redirect!=NULL && line->input_redirect==NULL;}
int IN_AND_OUT_REDIRECT(cmd_line* line){return line->input_redirect!=NULL && line->output_redirect!=NULL;}
int IS_NOT_NULL(cmd_line* line){return line!=NULL;}
int NO_NEXT(cmd_line* line){return line->next==NULL;}


void execute_execvp(cmd_line *line){
    if (execvp(line->arguments[0], line->arguments) == ERROR ) {
        perror("Error: ");
        _exit(ERROR_EXIT);
    }
}

void in_redirect_exec(cmd_line *line){
    fclose(stdin);
    FILE * stream = fopen(line->input_redirect,"r");
    if(!stream)
        perror("Error: CANNOT READ FILE");
    else 
        execute_execvp(line);
    fclose(stream);
}
void out_redirect_exec(cmd_line *line){
    fclose(stdout);
    FILE * stream = fopen(line->output_redirect,"w");
    if(!stream)
        perror("Error: CANNOT OPEN FILE");
    else
        execute_execvp(line);
    fclose(stream);
}
void in_and_out_redirect_exec(cmd_line *line){
    fclose(stdin);
    fclose(stdout);
    FILE * stream_in = fopen(line->input_redirect,"r");
    FILE * stream_out = fopen(line->output_redirect,"w");
    if(!stream_in)
        perror("Error: CANNOT READ FILE");
    else if(!stream_out)
        perror("Error: CANNOT OPEN FILE");
    else
        execute_execvp(line);
    fclose(stream_in);
    fclose(stream_out);
}

void no_redirect_exec(cmd_line *line){execute_execvp(line);}

void execute(cmd_line *line){
    if(IN_REDIRECT(line))         
        in_redirect_exec(line);
    else if(OUT_REDIRECT(line))         
        out_redirect_exec(line);
    else if(IN_AND_OUT_REDIRECT(line))  
        in_and_out_redirect_exec(line);
    else
        no_redirect_exec(line);
}

void close_and_dup(int fd_close, int fd_dup){
    close(fd_close);
    dup(fd_dup);
    close(fd_dup);
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
            if (NO_NEXT(cmd_parse)) {
                pid_t pid = fork();
                if (pid==ERROR) perror("Error: ");

                if (pid==CHILD)
                    execute(cmd_parse);
                else if(cmd_parse->blocking)
                    waitpid(pid, &status, 0);

            } else {
                int fd[2];
                if (pipe(fd) == ERROR)
                    perror("Error: PIPE FAILD");
                else {
                    pid_t pid1 = fork();
                    if (pid1==ERROR) perror("Error: ");

                    if (pid1 == CHILD) {
                        close_and_dup(1,fd[1]);
                        execute(cmd_parse);
                    } else
                        close(fd[1]);

                    pid_t pid2 = fork();
                    if (pid1 == ERROR) perror("Error: ");

                    if (pid2 == CHILD) {
                        close_and_dup(0,fd[0]);
                        execute(cmd_parse->next);
                    } else
                        close(fd[0]);

                    waitpid(pid1, &status, 0);
                    waitpid(pid2, &status, 0);
                }
            }
        }
        free_cmd_lines(cmd_parse);
    }
    return 0;
}
