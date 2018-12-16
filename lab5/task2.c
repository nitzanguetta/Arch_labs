#include <stdio.h>
#include <unistd.h>
#include <wait.h>

#define CHILD 0
#define ERROR -1

int main (int argc, char *argv[]) {
    int fd[2];
    if (pipe(fd)==-1)
        perror("Error: PIPE FAILD");
    else {
        pid_t pid1 = fork();
        (pid1==ERROR)? perror("Error1: ") : 0;

        if (pid1==CHILD) {
            close(1);
            dup(fd[1]);
            close(fd[1]);
            
            char *args[] = {"ls", "-l",0};
            execvp(args[0], args);
        }else {
            close(fd[1]);
        }

        pid_t pid2 = fork();
        (pid1==ERROR) ? perror("Error2: ") : 0;

        if (pid2==CHILD) {
            close(0);
            dup(fd[0]);
            close(fd[0]);

            char *args[] = {"tail", "-n", "2",0};
            execvp(args[0], args);
        } else {
            close(fd[0]);
        }
        int status1, status2;
        waitpid(pid1, &status1, 0);
        waitpid(pid2, &status2, 0);
    }
    return 0;
}