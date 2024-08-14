#include "../include/hashtable.h"
#include <assert.h>

typedef struct {
        char *key;
        float value;
} key_value_t;

// Examples
int main() {
        hash_table_t *ht = hash_table_create(1);
        // Force the hash table to not dynamically resized
        // hash_table_set_resize_flag(0);

        // String usage
        char key1_str[] = "key1_str";
        char val1_str[] = "val1";

        int result = hash_table_set(ht, key1_str, val1_str, sizeof(val1_str));
        assert(result == 0);
        char *res_str = hash_table_get(ht, key1_str);
        assert(res_str != NULL);
        assert(strcmp(res_str, val1_str) == 0);
        printf("result string: %s\n", res_str);

        // Int usage
        char key2_int[] = "key2_int";
        int val2_int = 2;

        result = hash_table_set(ht, key2_int, &val2_int, sizeof(val2_int));
        assert(result == 0);
        int *res_int = hash_table_get(ht, key2_int);
        assert(res_int != NULL);
        assert(*res_int == val2_int);
        printf("result int: %d\n", *res_int);

        // Float usage
        char key3_float[] = "key3_float";
        float val3_float = 3.14;

        result =
            hash_table_set(ht, key3_float, &val3_float, sizeof(val3_float));
        assert(result == 0);
        float *res_float = hash_table_get(ht, key3_float);
        assert(res_float != NULL);
        assert(*res_float == val3_float);
        printf("result float: %f\n", *res_float);
        assert(ht->size == 3);

        int delete_result = hash_table_remove(ht, key3_float);
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
                       sizeof(value_custom_kv));
        assert(ht->size == 3);
        key_value_t *res_custom_kv = hash_table_get(ht, key1_custom_kv);
        assert(value_custom_kv.key == res_custom_kv->key);
        assert(value_custom_kv.value == res_custom_kv->value);

        // Other examples
        char *res_doesnt_exist = hash_table_get(ht, "doesntexist");
        assert(res_doesnt_exist == NULL);

        // Print a list of keys for debugging
        hash_table_print_keys(ht);

        hash_table_cleanup(ht);
}
