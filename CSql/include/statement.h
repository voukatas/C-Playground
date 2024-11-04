#ifndef STATEMENT_H
#define STATEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/meta_command.h"

typedef enum {
    STATEMENT_COMMAND_SUCCESS,
    STATEMENT_COMMAND_UNKNOWN_COMMAND
} statement_command_result_t;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } statement_type_t;

typedef struct statement {
    statement_type_t type;
} statement_t;

statement_command_result_t prepare_statement(char *stmt_cmd, statement_t *stmt);
void execute_statement(statement_t *stmt);

#endif
