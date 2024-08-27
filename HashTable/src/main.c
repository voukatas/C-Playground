#include "../include/hashtable.h"
#include <assert.h>

typedef struct {
        char *key;
        float value;
} key_value_t;

typedef struct {
        char *key;
        void *value;
} key_extra_value_t;

void custom_cleanup(void *arg) {
        key_extra_value_t *key_extra_value = arg;
        free(key_extra_value->value);
}
// Examples
int main() {
        hash_table_t *ht = hash_table_create(1);
        // Force the hash table to not dynamically resized
        // hash_table_set_resize_flag(0);

        // String usage
        char key1_str[] = "key1_str";
        char val1_str[] = "val1";

        int result =
            hash_table_set(ht, key1_str, val1_str, strlen(val1_str) + 1,
                           NULL); // When you use strings use strlen
        assert(result == 0);
        char *res_str = hash_table_get(ht, key1_str);
        assert(res_str != NULL);
        assert(strcmp(res_str, val1_str) == 0);
        printf("result string: %s\n", res_str);

        // Int usage
        char key2_int[] = "key2_int";
        int val2_int = 2;

        result =
            hash_table_set(ht, key2_int, &val2_int, sizeof(val2_int), NULL);
        assert(result == 0);
        int *res_int = hash_table_get(ht, key2_int);
        assert(res_int != NULL);
        assert(*res_int == val2_int);
        printf("result int: %d\n", *res_int);

        // Float usage
        char key3_float[] = "key3_float";
        float val3_float = 3.14;

        result = hash_table_set(ht, key3_float, &val3_float, sizeof(val3_float),
                                NULL);
        assert(result == 0);
        float *res_float = hash_table_get(ht, key3_float);
        assert(res_float != NULL);
        assert(*res_float == val3_float);
        printf("result float: %f\n", *res_float);
        assert(ht->size == 3);

        int delete_result = hash_table_remove(ht, key3_float, NULL);
        assert(delete_result == 0);
        assert(ht->size == 2);

        // Verify the entry was deleted
        float *deleted_item = hash_table_get(ht, key3_float);
        assert(deleted_item == NULL);

        // Custom struct usage
        // We store in the key a custom struct
        char key1_custom_kv[] = "key1_custom_kv";
        key_value_t value_custom_kv;

        value_custom_kv.key = "inner_key";
        value_custom_kv.value = 3.14;

        hash_table_set(ht, key1_custom_kv, &value_custom_kv,
                       sizeof(value_custom_kv), NULL);
        assert(ht->size == 3);
        key_value_t *res_custom_kv = hash_table_get(ht, key1_custom_kv);
        assert(value_custom_kv.key == res_custom_kv->key);
        assert(value_custom_kv.value == res_custom_kv->value);

        // Other examples
        char *res_doesnt_exist = hash_table_get(ht, "doesntexist");
        assert(res_doesnt_exist == NULL);

        // Print a list of keys for debugging
        long num_of_keys = hash_table_print_keys(ht);
        printf("Num of keys: %ld\n", num_of_keys);

        hash_table_cleanup(ht, NULL);

        // Different scenario with new hash table
        // We store in the key a custom struct which also allocates extra memory
        // and needs handling
        ht = hash_table_create(1);

        char key2_extra_custom_kv[] = "key2_extra_custom_kv";
        key_extra_value_t value_extra_custom_kv;

        value_extra_custom_kv.key = "inner_extra_key";
        value_extra_custom_kv.value = strdup("inner_extra_value");

        // re-apply the same key
        // char key3_extra_custom_kv[] = "key3_extra_custom_kv";
        key_extra_value_t value_extra_custom_kv3;

        value_extra_custom_kv3.key = "inner_extra_key";
        value_extra_custom_kv3.value = strdup("inner_extra_value3");

        hash_table_set(ht, key2_extra_custom_kv, &value_extra_custom_kv,
                       sizeof(value_extra_custom_kv), custom_cleanup);
        hash_table_set(ht, key2_extra_custom_kv, &value_extra_custom_kv3,
                       sizeof(value_extra_custom_kv3), custom_cleanup);
        assert(ht->size == 1);
        key_extra_value_t *res_extra_custom_kv =
            hash_table_get(ht, key2_extra_custom_kv);
        assert(value_extra_custom_kv3.key == res_extra_custom_kv->key);
        assert(strcmp(value_extra_custom_kv3.value,
                      res_extra_custom_kv->value) == 0);
        hash_table_remove(ht, key2_extra_custom_kv, custom_cleanup);

        num_of_keys = hash_table_print_keys(ht);
        printf("Num of keys: %ld\n", num_of_keys);

        hash_table_cleanup(ht, custom_cleanup);
}
