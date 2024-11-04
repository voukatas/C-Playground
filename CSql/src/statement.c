#include "../include/statement.h"

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
