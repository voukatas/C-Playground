// gcc src/main.c -Iinclude -Wall -Wextra -Werror -g -O0 && ./a.out
//
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// enums
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNKNOWN_COMMAND
} meta_command_result_t;

typedef enum {
    STATEMENT_COMMAND_SUCCESS,
    STATEMENT_COMMAND_UNKNOWN_COMMAND
} statement_command_result_t;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } statement_type_t;

typedef struct statement {
    statement_type_t type;
} statement_t;

// function declarations
int run_repl(void);
void trim(char *cmd);
meta_command_result_t run_meta_command(char *meta_cmd);
statement_command_result_t prepare_statement(char *stmt_cmd, statement_t *stmt);
void execute_statement(statement_t *stmt);

int main(void) { return run_repl(); }

int run_repl(void) {
    while (true) {
        char cmd[256];
        printf("csql> ");
        if (fgets(cmd, sizeof(cmd), stdin) != NULL) {
            cmd[strcspn(cmd, "\n")] = '\0';
            trim(cmd);

            // check if it is meta-command
            if (cmd[0] == '.') {
                meta_command_result_t meta_res = run_meta_command(cmd);
                switch (meta_res) {
                    case META_COMMAND_SUCCESS:
                        continue;
                    case META_COMMAND_UNKNOWN_COMMAND:
                        continue;
                }
            }

            // regular sql commnad handling
            statement_t statement;
            statement_command_result_t stmt_result =
                prepare_statement(cmd, &statement);
            switch (stmt_result) {
                case STATEMENT_COMMAND_SUCCESS:
                    break;
                case STATEMENT_COMMAND_UNKNOWN_COMMAND:
                    printf("Unknown command '%s'\n", cmd);
                    continue;
            }

            execute_statement(&statement);
            printf("Statement executed\n");
        }
    }

    return 0;
}

void execute_statement(statement_t *stmt) {
    switch (stmt->type) {
        case STATEMENT_SELECT:
            printf("Execute select statement\n");
            break;
        case STATEMENT_INSERT:
            printf("Execute insert statement\n");
            break;
    }
}

statement_command_result_t prepare_statement(char *stmt_cmd,
                                             statement_t *stmt) {
    if (strncmp(stmt_cmd, "insert", 6) == 0) {
        stmt->type = STATEMENT_INSERT;
        return STATEMENT_COMMAND_SUCCESS;

    } else if (strncmp(stmt_cmd, "select", 6) == 0) {
        stmt->type = STATEMENT_SELECT;
        return STATEMENT_COMMAND_SUCCESS;
    }

    return STATEMENT_COMMAND_UNKNOWN_COMMAND;
}

meta_command_result_t run_meta_command(char *meta_cmd) {
    if (strcmp(meta_cmd, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        printf("Not recognized command\n");
        return META_COMMAND_UNKNOWN_COMMAND;
    }

    return META_COMMAND_SUCCESS;
}

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
