#include "../include/hashtable.h"

void checkSetResult(int result) {
        if (result != 0) {
                printf("%s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
}

int main() {
        hash_table_t *ht = create_table(100);
        char key1[] = "key1";
        char key2[] = "key2";
        char val1[] = "val1";
        char val2[] = "val2";
        char val11[] = "val11";

        int result = set_value(ht, key1, val1);
        checkSetResult(result);
        char *res1 = get_value(ht, key1);
        printf("res1: %s\n", res1);

        result = set_value(ht, key2, val2);
        checkSetResult(result);
        char *res2 = get_value(ht, key2);
        printf("res2: %s\n", res2);

        result = set_value(ht, key1, val11);
        checkSetResult(result);
        char *res3 = get_value(ht, key1);
        printf("res3: %s\n", res3);
        char *res4 = get_value(ht, "doesntexist");
        printf("res4: %s\n", res4);

        print_keys(ht);

        // resize(ht);

        clean_up(ht);
}
