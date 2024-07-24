#define TESTING
#include "../include/hashtable.h"
#include "../include/mock_malloc.h"
#include "../unity/unity.h"

void setUp(void) {
}

void tearDown(void) {
}

// Test cases
void test_hashtable_with_capacity_one(void) {
        hash_table_t *ht = create_table(1);
        char key1[] = "key1";
        char key2[] = "key2";
        char val1[] = "val1";
        char val2[] = "val2";
        char val11[] = "val11";

        set_value(ht, key1, val1);
        char *res1 = get_value(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val1, res1);

        set_value(ht, key2, val2);
        char *res2 = get_value(ht, key2);
        TEST_ASSERT_EQUAL_STRING(val2, res2);

        set_value(ht, key1, val11);
        char *res3 = get_value(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val11, res3);

        // PrintKeys(ht);
        clean_up(ht);
}

void test_hashtable_with_capacity_100(void) {
        hash_table_t *ht = create_table(100);
        char key1[] = "key1";
        char key2[] = "key2";
        char key3[] = "doesntexist";
        char val1[] = "val1";
        char val2[] = "val2";
        char val11[] = "val11";

        set_value(ht, key1, val1);
        char *res1 = get_value(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val1, res1);

        set_value(ht, key2, val2);
        char *res2 = get_value(ht, key2);
        TEST_ASSERT_EQUAL_STRING(val2, res2);

        set_value(ht, key1, val11);
        char *res3 = get_value(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val11, res3);

        // Test that NULL is returned in case a key doesn't exist
        char *res4 = get_value(ht, key3);
        TEST_ASSERT_NULL(res4);

        // PrintKeys(ht);
        clean_up(ht);
}

void test_Set_malloc_fail(void) {
        int table_size = 100;
        hash_table_t *ht = create_table(table_size);

        // Simulate malloc failure
        set_malloc_fail(1);

        char key[] = "key";
        char val[] = "val";
        int result = set_value(ht, key, val);

        TEST_ASSERT_EQUAL_INT(-1, result);

        // Verify that the value was not set
        char *res = get_value(ht, key);
        TEST_ASSERT_NULL(res);

        // Clean up
        set_malloc_fail(0);
        clean_up(ht);
}

int main(void) {
        UNITY_BEGIN();
        RUN_TEST(test_hashtable_with_capacity_one);
        RUN_TEST(test_hashtable_with_capacity_100);
        RUN_TEST(test_Set_malloc_fail);
        return UNITY_END();
}
