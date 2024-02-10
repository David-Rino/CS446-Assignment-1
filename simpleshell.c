#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int parseInput(char* input, char* tokens[]) {
    int tokenCount = 0;

    //printf(" %s\n", input);

    if (input == NULL) {
        return -1;
    }

    tokens[tokenCount] = strtok(input, " \n");
    printf(" %s\n", tokens[tokenCount]);
    //printf(" %d\n", tokenCount);

    while(tokens[tokenCount] != NULL) {
        tokens[++tokenCount] = strtok(NULL, " /n");
        printf(" %d\n", tokenCount);
        printf(" %s\n", tokens[tokenCount]);
    }

    tokens[tokenCount] = NULL;
    printf(" %d\n", tokenCount);
    printf(" %s\n", tokens[tokenCount]);
    return tokenCount;

}

int main() {
    char cliInput[1024], *cmd[1024];
    printf("something test\n");
    fgets(cliInput, 1024, stdin);
    int inp = parseInput(cliInput, cmd);
}
