#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

char* getHost();
char* getCMD(char*, char*);
int parseInput(char*, char**);
int executeCommand(char*, char**);
int changeDirectories(const char*);
int getDir(char*, size_t);

int main() {
    
    char* machine_name = getHost(); // Get the machine name
    if (machine_name == NULL) { 
        // if machine name fails we should report error
        fprintf(stderr, "Error retrieving machine name.\n");
        return 1;
    }

    char inputtage[1024];
    //working array mem allocation

    while (1) { // the loop you asked for
        if (getDir(inputtage, sizeof(inputtage)) != 0) {
            fprintf(stderr, "Error retrieving current working directory.\n");
            return 1;
        } //error checks

        if (inputtage == NULL) {
            fprintf(stderr, "Error retrieving current working directory.\n");
            return 1;
        } //error checks

        char* input = getCMD(inputtage, machine_name);
        //grabs command from user

        if (input != NULL) {
            char* tokens[64]; //initialize to max 64, 64 is even a lot
            int token_count = parseInput(input, tokens);
            //find how many actual tokens we have by parseing the input

            if (token_count > 0) { //if we have a command to do:
                if (strcmp(tokens[0], "cd") == 0) {
                    //cd command
                    if (token_count != 2) {
                        fprintf(stderr, "Path Not Formatted Correctly!\n");
                    } else {
                        int result = changeDirectories(tokens[1]);
                        if (result != 0) {
                            fprintf(stderr, "Error changing directory.\n");
                        }
                    }
                } else if (strcmp(tokens[0], "exit") == 0) {
                    //exit command
                    //frees input for loop
                    free(input);
                    break;
                } else {
                    //other commands
                    int result = executeCommand(tokens[0], tokens);
                    if (result != 0) {
                        //if we don't find it using the C linux library it's probably not a command
                        fprintf(stderr, "Error executing command.\n");
                    }
                }
            }
            //if not in exit free input for next loop
            free(input);
        }
    }
    //free machine for mem leakage
    free(machine_name);

    return 0;
}

// ~~~ MANDATORY FUNCTIONS ~~~ \\'

int parseInput(char* input, char* tokens[]) {
    int token_count = 0;
    if (input == NULL) { return -1;}

    //use strtok to split the input into tokens based on whitespace
    char* token = strtok(input, " \t\n");

    while (token != NULL) {
        if (token_count >= 64) {
            fprintf(stderr, "Too many tokens (max %d)\n", 64 - 1);
            return -1;
        }

        tokens[token_count++] = strdup(token);
        token = strtok(NULL, " \t\n");
    }

    if (token_count == 0) {
        fprintf(stderr, "No command provided.\n");
        return -1;
    }

    //set the last element to NULL to indicate the end
    tokens[token_count] = NULL;

    return token_count;
}

int executeCommand(char* command, char* args[]) {
    pid_t pid = fork(); //forks

    if (pid == -1) { 
        perror("fork Failed");
        //if failed
        return -1;
    } else if (pid == 0) { 
        //success
        //runs child
        if (execvp(command, args) == -1) {
            fprintf(stderr, "exec Failed: %s\n", strerror(errno));
            _exit(1); 
            //if failure terminates child
        }
    } else {
        //parent process
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            //if wait fail report error
            perror("wait Failed");
            return -1;
        }

        if (WIFEXITED(status)) {
            //as child process dies reports
            return WEXITSTATUS(status); 
            //exit status of chaild, if abnormal returns abnormality
        } else {
            fprintf(stderr, "Child Process Terminated Abnormally\n");
            return -1;
        }
    }
    return 0;
}

int changeDirectories(const char* path) {
    //super simple, find path if path doesnt exist or failed report it, if it does cd path
    if (path == NULL || strlen(path) == 0) {
        fprintf(stderr, "Path Not Formatted Correctly!\n");
        return -1;
    }

    if (chdir(path) == -1) {
        fprintf(stderr, "chdir Failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}


// ~~~ ADDITIONAL FUNCTIONS ~~~ \\'
// added for abstraction and simplification of main mostly
// not essential for program but makes it look nicer

int getDir(char* cwd, size_t size) {
    
    if (getcwd(cwd, size) == NULL) {
        //if it doesnt return a directiory (null) prints error
        perror("getcmd");
        return -1;
    }

    return 0;
}

char* getCMD(char* cwd, char* machine_name) {
    printf("domminicm@%s:%s$ ", machine_name, cwd);
    char input[1024]; //similar to main

    if (fgets(input, sizeof(input), stdin) == NULL) {
        perror("fgets");
        return NULL;
    }

    //remove \n, i believe it's at the end from input
    size_t input_length = strlen(input);
    if (input_length > 0 && input[input_length - 1] == '\n') {
        input[input_length - 1] = '\0';
    }

    return strdup(input);
}

char* getHost() {
    size_t size = 64;
    //allocate space for size buffer
    char* name = (char*)malloc(size);

    if (name != NULL) {
        if (gethostname(name, size) != 0) {
            perror("gethostname");
            free(name);
            return NULL;
        }
    }

    return name;
}