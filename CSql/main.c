// gcc main.c -Wall -Wextra -Werror -g -O0 && ./a.out
//
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// function declarations
void trim(char *cmd);

int main(void) {
    while (true) {
        char cmd[256];
        printf("csql> ");
        if (fgets(cmd, sizeof(cmd), stdin) != NULL) {
            cmd[strcspn(cmd, "\n")] = '\0';
            trim(cmd);
            
            // check command
            if (strcmp(cmd, ".exit") == 0) {
                return 0;
            } else {
                printf("Not recognized command\n");
            }
        }
    }

    return 0;
}

void trim(char *cmd){
    char *start = cmd;

    // remove from front
    while(*start && isspace(*start)){
        start++;
    }

    memmove(cmd, start, strlen(start)+1);

    // trail
    char *end = cmd + strlen(cmd)-1;
    while(end>start && isspace(*end)){
        *end='\0';
        end--;
    }
}
