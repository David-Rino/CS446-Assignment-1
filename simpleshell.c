#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>



int parseInput(char* input, char* tokens[]) {
    int tokenCount = 0;

    //printf(" %s\n", input);

    if (input == NULL) {
        return -1;
    }

    tokens[tokenCount] = strtok(input, " \n");
    //printf(" %s\n", tokens[tokenCount]);
    //printf(" %d\n", tokenCount);

    while(tokens[tokenCount] != NULL) {
        tokens[++tokenCount] = strtok(NULL, " \n");
        //printf(" %d\n", tokenCount);
        //printf(" %s\n", tokens[tokenCount]);
    }

    tokens[tokenCount] = NULL;
    //printf(" %d\n", tokenCount);
    //printf(" %s\n", tokens[5]);
    return tokenCount;

}

int executeCommand(char* command, char* arguments[]) {
    pid_t pid = fork();
    int status;

    if (pid == -1) {
        perror("Error: fork Failed");
        return -1;
    }

    if (pid == 0) {
        if (execvp(command, arguments) == -1) {
            printf("Exec error");
            _exit(1);
        }
    } else if(waitpid(pid, &status, 0) == -1) {
        perror("Error: Wait Failed");
        return -1;
    } else if(WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        printf("Child ended \n");
        return -1;
    }

    return 0;

}

int main() {
    char cliInput[1024], *cmd[1024];
    printf("something test\n");
    fgets(cliInput, 1024, stdin);
    int inp = parseInput(cliInput, cmd);
    int failed = executeCommand(cmd[0], cmd);
}
