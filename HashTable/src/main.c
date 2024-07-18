#include "../include/hashtable.h"

int main() {
        HashTable *ht = createTable(100);
        char key1[] = "key1";
        char key2[] = "key2";
        char val1[] = "val1";
        char val2[] = "val2";
        char val11[] = "val11";

        Set(ht, key1, val1);
        char *res1 = Get(ht, key1);
        printf("res1: %s\n", res1);

        Set(ht, key2, val2);
        char *res2 = Get(ht, key2);
        printf("res2: %s\n", res2);

        Set(ht, key1, val11);
        char *res3 = Get(ht, key1);
        printf("res3: %s\n", res3);

        PrintKeys(ht);

        CleanUp(ht);
}
