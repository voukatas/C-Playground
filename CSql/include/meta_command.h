#ifndef META_COMMAND_H
#define META_COMMAND_H

#include "../include/util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNKNOWN_COMMAND
} meta_command_result_t;

meta_command_result_t run_meta_command(char *meta_cmd);

#endif
