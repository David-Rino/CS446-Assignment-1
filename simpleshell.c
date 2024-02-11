//Author: Rino David
//Class: CS 446
//Purpose: Create a simpleshell simulation

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

    if (input == NULL) {
        return -1;
    }

    tokens[tokenCount] = strtok(input, " \n");

    while(tokens[tokenCount] != NULL) {
        tokens[++tokenCount] = strtok(NULL, " \n");
    }

    tokens[tokenCount] = NULL;
    return tokenCount;

}

int executeCommand(char* command, char* arguments[]) {
    pid_t pid = fork();
    int status;

    if (pid == -1) {
        //fork failed error.
        perror("fork Failed");
        return -1;
    }

    if (pid == 0) {
        if (execvp(command, arguments) == -1) {
            //If exec failed report error via perror and also terminates the child.
            printf("Exec error: %s", strerror(errno));
            _exit(1);
        }
    } else if(waitpid(pid, &status, 0) == -1) {
        //If the wait failed report error via perror.
        perror("Child finished with error status:");
        return -1;
    } else if(WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        printf("Child ended with for abnromal reasons \n");
        return -1;
    }

    return 0;

}

int changeDirectories(char* path) {
    //Add a check for trainling arguments
    if (path == NULL) {
        printf("Path Not Formatted Correctly!\n");
        return -1;
    } else if (chdir(path) == -1) {
        printf("chdir Failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int main() {
    char input[1024], *cmd[1024], cwd[1024];

    while(1) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            printf("Error getting current directory");
            return -1;
        }

        printf("rinod:%s$", cwd);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error fgets");
        }

        if (input != NULL) {
            int tokenNo = parseInput(input, cmd);

            //We now check for our special "edge case" commands cd and exit
            if (tokenNo > 0) {
                if (strcmp(cmd[0], "cd") == 0) {
                    if (tokenNo > 2) {
                        printf("Erorr: To many trailing agruments\n");
                    } else {
                        int dirResult = changeDirectories(cmd[1]);
                        if (dirResult != 0) {printf("Failed to change directory\n");}
                    }
                } else if (strcmp(cmd[0], "exit") == 0) {
                    //make sure to free all our allocated memory before exiting
                
                    break;
                } else {
                    int commandResult = executeCommand(cmd[0], cmd);
                    if (commandResult != 0) {
                        printf("Error when executing command\n");
                    }
                }
            }
        }
    }

    //Not entirely sure if I should be freeing my variables as when I do it gives "warning: ‘void free(void*)’ called on unallocated object"
    //free(input);
    //free(cmd);
    //free(cwd);

    return 0;

}
