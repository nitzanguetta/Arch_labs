#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include "line_parser.h"
#include "job_control.h"

#define PATH_MAX        4096
#define CHILD           0
#define ERROR           -1

int IS_NEWLINE(char* input) {return !strcmp("\n",input);}
int IS_JOBS(char* input) {return !strcmp("jobs\n",input);}
int IS_QUIT(char* input) {return !strcmp("quit\n",input);}
int IS_BG(cmd_line* line) {return !strcmp("bg",line->arguments[0]);}
int IS_FG(cmd_line* line) {return !strcmp("fg",line->arguments[0]);}
int IN_REDIRECT(cmd_line* line){return line->input_redirect!=NULL && line->output_redirect==NULL;}
int OUT_REDIRECT(cmd_line* line){return line->output_redirect!=NULL && line->input_redirect==NULL;}
int IN_AND_OUT_REDIRECT(cmd_line* line){return line->input_redirect!=NULL && line->output_redirect!=NULL;}
int IS_NOT_NULL(cmd_line* line){return line!=NULL;}
int NO_NEXT(cmd_line* line){return line->next==NULL;}


void execute_execvp(cmd_line *line){
    if (execvp(line->arguments[0], line->arguments) == ERROR ) {
        perror("Error: ");
        _exit(0x55);
    }
}

void in_redirect_exec(cmd_line *line){
    fclose(stdin);
    FILE * stream = fopen(line->input_redirect,"r");
    (!stream) ? perror("Error: CANNOT READ FILE"):0;
}
void out_redirect_exec(cmd_line *line){
    fclose(stdout);
    FILE * stream = fopen(line->output_redirect,"w");
    (!stream) ? perror("Error: CANNOT OPEN FILE"):0;
}
void in_and_out_redirect_exec(cmd_line *line){
    fclose(stdin);
    fclose(stdout);
    FILE * stream_in = fopen(line->input_redirect,"r");
    FILE * stream_out = fopen(line->output_redirect,"w");
    (!stream_in) ?  perror("Error: CANNOT READ FILE"):
    (!stream_out) ? perror("Error: CANNOT OPEN FILE"):0;
}

void no_redirect_exec(cmd_line *line){execute_execvp(line);}

void close_and_dup(int fd_close, int fd_dup){
    close(fd_close);
    dup(fd_dup);
//     close(fd_dup);
}

int execute(cmd_line *line , int left_pipe[2], int right_pipe[2], job * curr_job, pid_t _pid, job** job_list,struct termios* termios1) {
    pid_t pid = fork();
    if (pid == ERROR) return -1;
    if (pid == CHILD) {
        
        signal(SIGQUIT,SIG_DFL);
        signal(SIGCHLD,SIG_DFL);
        signal(SIGTTIN,SIG_DFL);
        signal(SIGTTOU,SIG_DFL);
        signal(SIGTSTP,SIG_DFL);
        
        setpgid(0,getpid());
        
        if(curr_job->pgid == 0)
            curr_job->pgid = getpgid(0);


        if (right_pipe != NULL)     close_and_dup(1, right_pipe[1]);
        if (left_pipe != NULL)      close_and_dup(0, left_pipe[0]);


        if (IN_REDIRECT(line))
            in_redirect_exec(line);
        else if(OUT_REDIRECT(line))
            out_redirect_exec(line);
        else if(IN_AND_OUT_REDIRECT(line))
            in_and_out_redirect_exec(line);

        execute_execvp(line);
        exit(0);

    } else {
        if(curr_job->pgid == 0){
            curr_job->pgid = pid;
            setpgid(0, pid);
        }

        if (right_pipe)     close(right_pipe[1]);
        if (left_pipe)      close(left_pipe[0]);


        if (line->blocking)
            run_job_in_foreground(job_list, curr_job, 0, termios1, getpgid(0));
        else
            run_job_in_background(curr_job,0);
        

        if (line->next) {
            if(line->next->next) {
                int fd[2];
                if (pipe(fd) == ERROR) {
                    perror("Error: PIPE FAILD");
                    exit(ERROR);
                }
                if (execute(line->next, right_pipe, fd,curr_job,_pid,job_list,termios1)== ERROR){
                    perror("Error:");
                    exit(ERROR);
                }
            }else{
                if (execute(line->next, right_pipe, NULL,curr_job,_pid,job_list,termios1)== ERROR){
                    perror("Error:");
                    exit(ERROR);
                }
            }
        }

    }
    return 0;
}

void print_curr_dir(){
    char buffer_pwd[PATH_MAX];
    printf("%s>", getcwd(buffer_pwd, PATH_MAX));
}

void handleSignal(int signalNum){
    //printf("%s:was signal\n",strsignal(signalNum));
}

int main (int argc, char *argv[])
{
    signal(SIGTTIN,SIG_IGN);
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGQUIT, handleSignal);
    signal(SIGCHLD, handleSignal);
    setpgid(0,0);

    struct termios *termios1= malloc(sizeof(struct termios));
    tcgetattr(STDIN_FILENO,termios1);

    char input[PATH_MAX];
    cmd_line *cmd_parse;
    job **job_list = malloc(sizeof(job*));

    while(1) {
        print_curr_dir();
        fgets(input, PATH_MAX, stdin);

        if(IS_NEWLINE(input)) continue;

        if(IS_QUIT(input)) break;
        else if (IS_JOBS(input)) {
            print_jobs(job_list);
            continue;
        }

        job *curr_job = add_job(job_list,input);
        curr_job->status=RUNNING;
        
        cmd_parse = parse_cmd_lines(input);
        
        
        if(IS_FG(cmd_parse)){
                run_job_in_foreground(job_list, find_job_by_index(*job_list,atoi(cmd_parse->arguments[1])), 1, termios1, getpid());
        }else if(IS_BG(cmd_parse)){
                run_job_in_background(find_job_by_index(*job_list,atoi(cmd_parse->arguments[1])),1);
        }else if (IS_NOT_NULL(cmd_parse)) {
            if (NO_NEXT(cmd_parse)) {
                execute(cmd_parse,NULL, NULL,curr_job,-1,job_list,termios1);
            } else {
                int fd[2];
                if (pipe(fd) == ERROR) perror("Error: PIPE FAILD");
                else {
                    if(execute(cmd_parse, NULL, fd,curr_job,-1,job_list,termios1) == ERROR)  perror("Error:");
                }
            }
        }
        free_cmd_lines(cmd_parse);
    }
    free(termios1);
    free_job_list(job_list);
    return 0;
}