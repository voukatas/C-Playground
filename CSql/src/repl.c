#include "../include/repl.h"

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
