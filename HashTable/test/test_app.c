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
        TEST_ASSERT_EQUAL_INT(1, ht->size);
        char *res1 = get_value(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val1, res1);

        set_value(ht, key2, val2);
        TEST_ASSERT_EQUAL_INT(2, ht->size);
        char *res2 = get_value(ht, key2);
        TEST_ASSERT_EQUAL_STRING(val2, res2);

        set_value(ht, key1, val11);
        TEST_ASSERT_EQUAL_INT(2, ht->size);
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
        TEST_ASSERT_EQUAL_INT(1, ht->size);
        char *res1 = get_value(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val1, res1);

        set_value(ht, key2, val2);
        TEST_ASSERT_EQUAL_INT(2, ht->size);
        char *res2 = get_value(ht, key2);
        TEST_ASSERT_EQUAL_STRING(val2, res2);

        set_value(ht, key1, val11);
        TEST_ASSERT_EQUAL_INT(2, ht->size);
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
        TEST_ASSERT_EQUAL_INT(0, ht->size);

        // Verify that the value was not set
        char *res = get_value(ht, key);
        TEST_ASSERT_NULL(res);

        // Clean up
        set_malloc_fail(0);
        clean_up(ht);
}

void test_delete_entry(void) {
        int table_size = 100;
        hash_table_t *ht = create_table(table_size);

        char key[] = "key";
        char val[] = "val";

        int set_result = set_value(ht, key, val);
        TEST_ASSERT_EQUAL_INT(1, ht->size);
        TEST_ASSERT_EQUAL_INT(0, set_result);

        char *res = get_value(ht, key);
        TEST_ASSERT_NOT_NULL(res);
        TEST_ASSERT_EQUAL_STRING(val, res);

        // Delete the entry
        int delete_result = delete_entry(ht, key);
        TEST_ASSERT_EQUAL_INT(0, ht->size);
        TEST_ASSERT_EQUAL_INT(0, delete_result);

        // Verify the entry was deleted
        res = get_value(ht, key);
        TEST_ASSERT_NULL(res);

        clean_up(ht);
}

void test_delete_invalid_entry(void) {
        int table_size = 100;
        hash_table_t *ht = create_table(table_size);

        char key[] = "key";

        // Delete the entry
        int delete_result = delete_entry(ht, key);
        TEST_ASSERT_EQUAL_INT(-1, delete_result);

        clean_up(ht);
}

void test_resize(void) {
        int table_size = 4;
        hash_table_t *ht = create_table(table_size);
        char key1[] = "key1";
        char key2[] = "key2";
        char key3[] = "key3";
        char key4[] = "key4";
        char key5[] = "key5";
        char val[] = "val";

        // set the values and check that the size increased
        set_value(ht, key1, val);
        TEST_ASSERT_EQUAL_INT(1, ht->size);

        set_value(ht, key2, val);
        TEST_ASSERT_EQUAL_INT(2, ht->size);

        set_value(ht, key3, val);
        TEST_ASSERT_EQUAL_INT(3, ht->size);

        set_value(ht, key4, val);
        TEST_ASSERT_EQUAL_INT(4, ht->size);

        set_value(ht, key5, val);
        TEST_ASSERT_EQUAL_INT(5, ht->size);

        // check that the capacity was increased
        TEST_ASSERT_TRUE(ht->capacity > table_size);

        // verify that the values still exist and they are spread in their
        // position
        char *res1 = get_value(ht, key1);
        TEST_ASSERT_EQUAL_STRING(val, res1);
        // TEST_ASSERT_EQUAL_STRING((ht->table[1])->key, key1);

        char *res2 = get_value(ht, key2);
        TEST_ASSERT_EQUAL_STRING(val, res2);
        // TEST_ASSERT_EQUAL_STRING((ht->table[2])->key, key2);

        char *res3 = get_value(ht, key3);
        TEST_ASSERT_EQUAL_STRING(val, res3);
        // TEST_ASSERT_EQUAL_STRING((ht->table[3])->key, key3);

        char *res4 = get_value(ht, key4);
        TEST_ASSERT_EQUAL_STRING(val, res4);
        // TEST_ASSERT_EQUAL_STRING((ht->table[4])->key, key4);

        char *res5 = get_value(ht, key5);
        TEST_ASSERT_EQUAL_STRING(val, res5);
        // TEST_ASSERT_EQUAL_STRING((ht->table[5])->key, key5);

        print_keys(ht);
        clean_up(ht);
}

void *thread_func1(void *arg) {
        hash_table_t *ht = (hash_table_t *)arg;
        for (int i = 0; i < 1000; ++i) {
                // pthread_mutex_lock(&counter_mutex);
                // set_value(ht, "key1", "val1");
                // pthread_mutex_unlock(&counter_mutex);
                set_value(ht, "key1", "val1");
        }
        return NULL;
}

void *thread_func2(void *arg) {
        hash_table_t *ht = (hash_table_t *)arg;
        for (int i = 0; i < 1000; ++i) {
                // pthread_mutex_lock(&counter_mutex);
                // set_value(ht, "key2", "val1");
                // pthread_mutex_unlock(&counter_mutex);
                set_value(ht, "key1", "val1");
        }
        return NULL;
}
// consider deleting this...
// check that valgrind doesn't report any data races
void test_threading(void) {
        int table_size = 4;
        hash_table_t *ht = create_table(table_size);

        pthread_t t1, t2;

        pthread_create(&t1, NULL, thread_func1, ht);
        pthread_create(&t2, NULL, thread_func2, ht);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

        print_keys(ht);
        clean_up(ht);
}

// a little of mess here, probably i should use a different file
#define NUM_THREADS 10
#define NUM_OPERATIONS 1000 // 5000

typedef struct thread_data {
        hash_table_t *ht;
        char key[32];
        char value[32];
} thread_data_t;

void *thread_set_value(void *arg) {
        thread_data_t *data = (thread_data_t *)arg;
        for (int i = 0; i < NUM_OPERATIONS; i++) {
                char key[64];
                snprintf(key, sizeof(key), "%s_%d", data->key, i);
                set_value(data->ht, key, data->value);
        }
        return NULL;
}

void *thread_get_value(void *arg) {
        thread_data_t *data = (thread_data_t *)arg;
        for (int i = 0; i < NUM_OPERATIONS; i++) {
                char key[64];
                snprintf(key, sizeof(key), "%s_%d", data->key, i);
                get_value(data->ht, key);
        }
        return NULL;
}

void test_thread_safety(void) {
        hash_table_t *ht = create_table(10);

        pthread_t threads[NUM_THREADS];
        thread_data_t thread_data[NUM_THREADS];

        // create threads to set
        for (int i = 0; i < NUM_THREADS / 2; i++) {
                snprintf(thread_data[i].key, sizeof(thread_data[i].key),
                         "key%d", i);
                snprintf(thread_data[i].value, sizeof(thread_data[i].value),
                         "value%d", i);
                thread_data[i].ht = ht;
                pthread_create(&threads[i], NULL, thread_set_value,
                               (void *)&thread_data[i]);
        }

        // create threads to use get
        for (int i = NUM_THREADS / 2; i < NUM_THREADS; i++) {
                snprintf(thread_data[i].key, sizeof(thread_data[i].key),
                         "key%d", i - NUM_THREADS / 2);
                snprintf(thread_data[i].value, sizeof(thread_data[i].value),
                         "value%d", i - NUM_THREADS / 2);
                thread_data[i].ht = ht;
                pthread_create(&threads[i], NULL, thread_get_value,
                               (void *)&thread_data[i]);
        }

        for (int i = 0; i < NUM_THREADS; i++) {
                pthread_join(threads[i], NULL);
        }

        // verify the values
        for (int i = 0; i < NUM_THREADS / 2; i++) {
                for (int j = 0; j < NUM_OPERATIONS; j++) {
                        char key[32];
                        snprintf(key, sizeof(key), "key%d_%d", i, j);
                        char *value = get_value(ht, key);
                        TEST_ASSERT_EQUAL_STRING(thread_data[i].value, value);
                }
        }

        // print_keys(ht);
        clean_up(ht);
}

int main(void) {
        UNITY_BEGIN();
        RUN_TEST(test_hashtable_with_capacity_one);
        RUN_TEST(test_hashtable_with_capacity_100);
        RUN_TEST(test_Set_malloc_fail);
        RUN_TEST(test_delete_entry);
        RUN_TEST(test_delete_invalid_entry);
        RUN_TEST(test_resize);
        RUN_TEST(test_threading);
        RUN_TEST(test_thread_safety);
        return UNITY_END();
}
