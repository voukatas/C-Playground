#include "../include/util.h"

void trim(char *cmd) {
    char *start = cmd;

    // remove from front
    while (*start && isspace(*start)) {
        start++;
    }

    memmove(cmd, start, strlen(start) + 1);

    // trail
    char *end = cmd + strlen(cmd) - 1;
    while (end > start && isspace(*end)) {
        *end = '\0';
        end--;
    }
}
