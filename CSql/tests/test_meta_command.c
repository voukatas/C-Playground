#include "../include/meta_command.h"
#include "../unity/unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_run_meta_command_exit(void) {
    meta_command_result_t result = run_meta_command(".exit");
    TEST_ASSERT_EQUAL(META_COMMAND_SUCCESS, result);
}

void test_run_meta_command_unknown(void) {
    meta_command_result_t result = run_meta_command(".unknown");
    TEST_ASSERT_EQUAL(META_COMMAND_UNKNOWN_COMMAND, result);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_run_meta_command_exit);
    RUN_TEST(test_run_meta_command_unknown);
    return UNITY_END();
}
