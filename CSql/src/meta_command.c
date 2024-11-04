#include "../include/meta_command.h"

meta_command_result_t run_meta_command(char *meta_cmd) {
    if (strcmp(meta_cmd, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        printf("Not recognized command\n");
        return META_COMMAND_UNKNOWN_COMMAND;
    }
    return META_COMMAND_SUCCESS;
}
