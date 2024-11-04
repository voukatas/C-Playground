#include "../include/statement.h"
#include "../unity/unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_prepare_statement_insert(void) {
    statement_t stmt;
    statement_command_result_t result = prepare_statement("insert", &stmt);
    TEST_ASSERT_EQUAL(STATEMENT_COMMAND_SUCCESS, result);
    TEST_ASSERT_EQUAL(STATEMENT_INSERT, stmt.type);
}

void test_prepare_statement_select(void) {
    statement_t stmt;
    statement_command_result_t result = prepare_statement("select", &stmt);
    TEST_ASSERT_EQUAL(STATEMENT_COMMAND_SUCCESS, result);
    TEST_ASSERT_EQUAL(STATEMENT_SELECT, stmt.type);
}

void test_prepare_statement_unknown(void) {
    statement_t stmt;
    statement_command_result_t result = prepare_statement("unknown", &stmt);
    TEST_ASSERT_EQUAL(STATEMENT_COMMAND_UNKNOWN_COMMAND, result);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_prepare_statement_insert);
    RUN_TEST(test_prepare_statement_select);
    RUN_TEST(test_prepare_statement_unknown);
    return UNITY_END();
}

