//shell

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
int status;

void clear_trash(char *list){
    free(list);
}

void clear_more_trash(char **list){
    for(int i = 0; list[i] != NULL; i++){
        clear_trash(list[i]);
    }
    free(list);
}

char *scan(){
    int bufsize = 1024;
    char *a = (char*)calloc(bufsize, sizeof(char));
    char tmp;
    int c, i = 0, j = 0;
    while(1){
        if(i >= bufsize){
            bufsize *= 2;
            a = realloc(a, bufsize); 
        }
        c = getchar();
        if (c == EOF || c == '\n') {
            a[i] = '\0';
            return a;
        }
        a[i] = c;
        i++;
    }
}

char *split(char *a){
    int bufsize = 1024;
    char *b = (char*)calloc(bufsize, sizeof(char));
    int i = 0, j = 0;
    int flag3 = 0;
    int flag1 = 0;
    int flag2 = 0;
    int flag4 = 0; 
    while(a[i] != '\0' ){
        if(i >= bufsize){
            bufsize *= 2;
            b = realloc(b, bufsize); 
        }
        switch (a[i]){
            case '"':
                if(flag1 == 0){
                    flag1 = 1;
                    i++;
                    break;
                }
                else{
                    flag1 = 0;
                    flag2 = 1;
                    i++;
                    continue;
                }
            case ' ':
                if(flag1 == 1)
                    break;
                if(flag2 == 1){
                    b[j] = '\n';
                    j++;
                }
                flag2 = 0;
                break;
            default:
                flag2 = 1;
                break;
            }
        if(flag1 == 1 || (flag1 == 0 && flag2 == 1)){
            b[j] = a[i];
            i++;
            j++;
            continue;
        }
        if(flag1 == 0 && flag2 == 0){
            i++;
            continue;
        }
    }
    clear_trash(a);
    return b;
}

char** check(char *a){
    int size = 1;
    int i = 0;
    char **tmp = (char**)calloc(1024, sizeof(char*));
    tmp[0] = (char*) calloc(256, sizeof(char));
    for(int j = 0; j < strlen(a); j++){
        if(a[j] == '\n'){
            size++;
            tmp = realloc(tmp, size);
            tmp[size - 1] = (char*) calloc(256, sizeof(char));
            i = 0;
            continue;
        }
        tmp[size - 1][i] = a[j];
        i++;
    }
    tmp[size] = NULL;
    clear_trash(a);
    return tmp;
}

char** check2(char **a){
    int i = 0;
    int size = 0;
    char **tmp = (char**) calloc(size, sizeof(char*));
    tmp[0] = (char*) calloc(256, sizeof(char));
    while(a[i] != NULL){
        if(strcmp(a[i], ">") == 0 || strcmp(a[i], ">>") == 0 || strcmp(a[i], "<") == 0 || strcmp(a[i], "|") == 0 || strcmp(a[i], "||") == 0){
            i++;
            continue;
        }
        size++;
        tmp = realloc(tmp, size * sizeof(char*));
        tmp[size - 1] = a[i];
        i++;
    }
    tmp = realloc(tmp, (size + 1) * sizeof(char*));
    tmp[size] = NULL;
    return tmp;
}

void handler(int sig) {
    wait(&status);
}

void help() {
    printf("Pls enter with spaces\n\n");
    printf("Inner functions:\n");
    printf("help, cd, echo, pwd, exit\n\n");
}

void exec(char **cmd) { 
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        if(execvp(cmd[0], cmd) < 0){
            printf("Invalid command: %s\n", cmd[0]);
        }
        exit(EXIT_FAILURE);
    } 
    else{
        wait(NULL);
    }
}

void exec0(char **cmd){        // >
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        int file = open(cmd[1], O_EXCL | O_RDONLY);
        if(file == -1){
            perror("open");
            exit(EXIT_FAILURE);
        }
        if(dup2(file, STDIN_FILENO) == -1){
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(file);
        execvp(cmd[0], cmd);
        perror("shell");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
}

void exec1(char **cmd){        // <
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        int file = open(cmd[1], O_CREAT | O_TRUNC | O_WRONLY, 0660);
        if(file == -1){
            perror("open");
            exit(EXIT_FAILURE);
        }
        if(dup2(file, STDOUT_FILENO) == -1){
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(file);
        execvp(cmd[0], cmd);
        perror("shell");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
}

void exec2(char **cmd){        // >>
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }    
    if(pid == 0){
        int file = open(cmd[1], O_WRONLY | O_APPEND | O_CREAT, 0666);
        if (file == -1){
            perror("open");
            exit(EXIT_FAILURE);
        }
        if(dup2(file, STDOUT_FILENO) == -1){
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(file);
        execvp(cmd[0], cmd);
        perror("shell");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
}

void cd(char **arr) {
    printf("%s\n", arr[1]);
    if(strcmp(arr[0], "cd") == 0){
        char *home = getenv("HOME");
        if(arr[1] == NULL || strcmp(arr[1], "~") == 0){
            chdir(home);
        } 
        else{
            chdir(arr[1]);
        }
    }
}

void echo(char **arr){
    if(strcmp(arr[1], "*") == 0){
        char **tmp;
        tmp[0] = "ls";
        tmp[1] = NULL;
        exec(tmp);
    }
    else{
        for(int i = 0; i < strlen(arr[1]); i++) {
            printf("%c", arr[1][i]);
        }
        puts("");
    }
}

void conv(char **cmd, int cmd_num){
    int fd[cmd_num - 1];
    pid_t pid;
    for (int i = 0; i < cmd_num; i++){
        pid = fork();
        if(pid == -1){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if(pid == 0){
            if(i != 0){
                if(dup2(fd[i - 1], STDIN_FILENO) == -1) {
                    perror("dup2s");
                    exit(EXIT_FAILURE);
                }
                close(fd[i - 1]);
            }
            if (i != cmd_num - 1){
                if (dup2(fd[i], STDOUT_FILENO) == -1) {
                      perror("dup2q");
                      exit(EXIT_FAILURE);
                }
                close(fd[i]);
            }
            execvp(cmd[i], cmd);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        if (i != 0){
            close(fd[i - 1]);
        }
        if (i != cmd_num - 1){    
            close(fd[i]);
        }
        wait(NULL);
    }
}


void exec3(char **cmd, int cmd_num) { // executes "||" bundle
    int status = 0;
    pid_t pid = fork();
    if (pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0){
        execvp(cmd[0], cmd);
        perror("fsh");
        exit(EXIT_FAILURE);
    }
    for (int i = 1; i < cmd_num; i++){
        wait(&status);
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            if (WEXITSTATUS(status) || WIFSIGNALED(status)) {
                pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                if (pid == 0) {
                    execvp(cmd[0], cmd);
                    perror("fsh");
                    exit(EXIT_FAILURE);
                }
                wait(NULL);
            }
        }
    }
}

int main(){
    help();
    while(!feof(stdin)){
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        printf("> ");
        char *line = scan();
        char *line2 = split(line);
        char **cmd = check(line2);      //каждое слово в отдельном элементе массива
        char **cmd2 = check2(cmd);      //каждое слово в отдельном элементе массива (убирает > >> < | ||)
        if(strcmp(cmd[0], "help") == 0){
            help();
            continue;
        }
        if(strcmp(cmd[0], "exit") == 0){
            return 0;
        }
        if(strcmp(cmd[0], "cd") == 0){
            cd(cmd2);
            continue;
        }
        if(strcmp(cmd[0], "pwd") == 0){
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            printf("Current working dir: %s\n", cwd);
            continue;
        }
        if(strcmp(cmd[0], "echo") == 0){
            echo(cmd);
            continue;
        }
        if(cmd[1] != NULL && strcmp(cmd[1], "|") == 0){
            printf("%s\n", cmd2[1]);
            printf("%s\n", cmd2[0]); //to check what was written
            conv(cmd2, 2);
            continue;
        }
        if(cmd[1] != NULL && strcmp(cmd[1], "||") == 0){
            exec3(cmd2, 2);
            continue;
        }
        if(cmd[1] != NULL && strcmp(cmd[1], "<") == 0){
            exec0(cmd2);
            continue;
        }
        if(cmd[1] != NULL && strcmp(cmd[1], ">") == 0){
            exec1(cmd2);
            continue;
        }
        if(cmd[1] != NULL && strcmp(cmd[1], ">>") == 0){
            exec2(cmd2);
            continue;
        }
        else{
            exec(cmd);
            continue;
        }
        clear_trash(line);
        clear_trash(line2);
        clear_more_trash(cmd);
        clear_more_trash(cmd2);
    }
    return 0;
}
