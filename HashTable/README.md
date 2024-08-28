# HashTable

A simple thread-safe HashTable implementation where the key is a string and the value can be any type of data. This implementation provides basic operations like set, get, remove, and resize with thread safety using pthread mutexes.

## How to Build
```bash
make
./hashtable_prod
# or run with debugging symbols
./hashtable_dbg
# or manually run with memory checks
valgrind --leak-check=full --track-origins=yes -s ./hashtable_dbg

```

# How to run tests
```bash
# Check for memory issues
valgrind --leak-check=full --track-origins=yes -s ./test_app
# Check for data races
valgrind --tool=helgrind ./test_app

# All in one
make clean && make && valgrind --leak-check=full --track-origins=yes -s ./test_app && valgrind --tool=helgrind ./test_app

```
## API Usage
### Creating and Destroying a Hash Table
- hash_table_create(int capacity): Create a new hash table with a specified initial capacity.
```c
hash_table_t *ht = hash_table_create(10); // Creates a hashtable with capacity 10

```
- hash_table_cleanup(hash_table_t *ht, void (*cleanup_callback)(void *)): Cleans up the hash table and frees all allocated memory. If a cleanup_callback is provided, it is used to clean up custom allocated memory within the stored values.
```c
hash_table_cleanup(ht, NULL); // Cleans up without any special handling

```
### Adding, Retrieving, and Removing Entries
- int hash_table_set(hash_table_t *ht, char *key, void *value, size_t size, void (*cleanup_callback)(void *)): Insert or update a value in the hash table. If cleanup_callback is provided, it will be called to clean up the previous value when it is replaced.
```c
char key[] = "example_key";
char value[] = "example_value";
hash_table_set(ht, key, value, strlen(value) + 1, NULL); // Set a string value

```
- void *hash_table_get(hash_table_t *ht, char *key): Retrieve a value from the hash table by its key. Returns NULL if the key is not found.

```c
char *retrieved_value = (char *)hash_table_get(ht, key);
if (retrieved_value) {
    printf("Retrieved value: %s\n", retrieved_value);
}

```
- int hash_table_remove(hash_table_t *ht, char *key, void (*cleanup_callback)(void *)): Remove an entry from the hash table by its key. If cleanup_callback is provided, it is called to clean up the value being removed.
```c
hash_table_remove(ht, key, NULL); // Remove the entry without any special handling

```
### Resizing and Debugging
- int hash_table_resize(hash_table_t *ht): Resizes the hash table when needed (internal use only).

- void hash_table_set_resize_flag(int enabled): Enable or disable automatic resizing of the hash table.

```c
hash_table_set_resize_flag(0); // Disable automatic resizing

```
- long hash_table_print_keys(hash_table_t *ht): Prints all keys in the hash table and returns the total number of keys.

```c
long num_of_keys = hash_table_print_keys(ht);
printf("Num of keys: %ld\n", num_of_keys);

```
## Example Usage

```c
#include "../include/hashtable.h"
#include <assert.h>

int main() {
    hash_table_t *ht = hash_table_create(1);

    // Example with strings
    char key1[] = "key1";
    char value1[] = "value1";
    hash_table_set(ht, key1, value1, strlen(value1) + 1, NULL);
    char *retrieved = hash_table_get(ht, key1);
    assert(retrieved && strcmp(retrieved, value1) == 0);

    // Example with integers
    char key2[] = "key2";
    int value2 = 42;
    hash_table_set(ht, key2, &value2, sizeof(value2), NULL);
    int *retrieved_int = (int *)hash_table_get(ht, key2);
    assert(retrieved_int && *retrieved_int == value2);

    // Clean up
    hash_table_cleanup(ht, NULL);

    return 0;
}

```
More detailed examples can be found inside the main.c
