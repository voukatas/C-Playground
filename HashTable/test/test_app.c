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
        HashTable *ht = createTable(1);
        char key1[] = "key1";
        char key2[] = "key2";
        char val1[] = "val1";
        char val2[] = "val2";
        char val11[] = "val11";

        Set(ht, key1, val1);
        char *res1 = Get(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val1, res1);

        Set(ht, key2, val2);
        char *res2 = Get(ht, key2);
        TEST_ASSERT_EQUAL_STRING(val2, res2);

        Set(ht, key1, val11);
        char *res3 = Get(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val11, res3);

        // PrintKeys(ht);
        CleanUp(ht);
}

void test_hashtable_with_capacity_100(void) {
        HashTable *ht = createTable(100);
        char key1[] = "key1";
        char key2[] = "key2";
        char val1[] = "val1";
        char val2[] = "val2";
        char val11[] = "val11";

        Set(ht, key1, val1);
        char *res1 = Get(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val1, res1);

        Set(ht, key2, val2);
        char *res2 = Get(ht, key2);
        TEST_ASSERT_EQUAL_STRING(val2, res2);

        Set(ht, key1, val11);
        char *res3 = Get(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val11, res3);

        // PrintKeys(ht);
        CleanUp(ht);
}

void test_Set_malloc_fail(void) {
        int table_size = 100;
        HashTable *ht = createTable(table_size);

        // Simulate malloc failure
        set_malloc_fail(1);

        char key[] = "key";
        char val[] = "val";
        Set(ht, key, val);

        // Verify that the value was not set
        char *res = Get(ht, key);
        TEST_ASSERT_NULL(res);

        // Clean up
        set_malloc_fail(0);
        CleanUp(ht);
}
int main(void) {
        UNITY_BEGIN();
        RUN_TEST(test_hashtable_with_capacity_one);
        RUN_TEST(test_hashtable_with_capacity_100);
        RUN_TEST(test_Set_malloc_fail);
        return UNITY_END();
}
