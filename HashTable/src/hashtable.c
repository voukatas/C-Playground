#ifdef TESTING
// #warning "TESTING macro is defined"
#include "../include/mock_malloc.h"
#endif

#include "../include/hashtable.h"

// init table
hash_table_t *create_table(int capacity) {
        hash_table_t *ht = malloc(sizeof(hash_table_t));
        if (!ht) {
                printf("failed to allocate memory: %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        ht->table = malloc(sizeof(entry_t *) * capacity);
        if (!ht->table) {
                printf("failed to allocate memory: %s", strerror(errno));
                free(ht);
                exit(EXIT_FAILURE);
        }

        ht->capacity = capacity;
        ht->size = 0;

        for (int i = 0; i < capacity; i++) {
                ht->table[i] = NULL;
        }

        return ht;
}

// Hash
int hash(char *key, int capacity) {
        unsigned int hash = 0;
        while (*key != '\0') {
                hash = (hash << 5) + *key++;
        }

        // printf("address: %d\n", hash% TABLE_SIZE);

        return hash % capacity;
}

// Set
int set_value(hash_table_t *ht, char *key, char *value) {
        // printf("init size: %d\n", ht->size);
        if (ht->size > (ht->capacity / 2)) {
                // printf("size: %d cap: %d\n", ht->size, ht->capacity / 2);
                int result = resize(ht);
                if (result != 0) {
                        return -1;
                }
        }
        int address = hash(key, ht->capacity);

        entry_t *entry = ht->table[address];

        while (entry != NULL) {
                if (strcmp(entry->key, key) == 0) {
                        // printf("entry key: %s key: %s\n", entry->Key, key);
                        free(entry->value);
                        entry->value = strdup(value);
                        if (!entry->value) {
                                printf("failed to allocate memory: %s\n",
                                       strerror(errno));
                                return -1;
                        }
                        return 0; // Success
                }
                entry = entry->next;
        }

        entry = malloc(sizeof(entry_t)); // consider also calloc
        if (!entry) {
                printf("failed to allocate memory: %s\n", strerror(errno));
                return -1;
        }
        entry->key = strdup(key);
        entry->value = strdup(value);
        if (!entry->key || !entry->value) {
                printf("failed to allocate memory for key or value: %s\n",
                       strerror(errno));
                free(entry->key);
                free(entry->value);
                free(entry);
                return -1; // Error
        }

        entry->next = ht->table[address];
        ht->table[address] = entry;

        ht->size++;
        // printf("---increase size: %d\n", ht->size);

        return 0;
}

// Get
char *get_value(hash_table_t *ht, char *key) {
        int address = hash(key, ht->capacity);
        entry_t *entry = ht->table[address];

        while (entry != NULL) {
                if (strcmp(entry->key, key) == 0) {
                        return entry->value;
                }
                entry = entry->next;
        }

        return NULL;
}

int delete_entry(hash_table_t *ht, char *key) {
        int address = hash(key, ht->capacity);

        entry_t *current_entry = ht->table[address];
        entry_t *prev_entry = NULL;

        while (current_entry != NULL) {
                if (strcmp(current_entry->key, key) == 0) {
                        if (prev_entry == NULL) {
                                ht->table[address] = current_entry->next;
                        } else {
                                prev_entry->next = current_entry->next;
                        }
                        free(current_entry->key);
                        free(current_entry->value);
                        free(current_entry);
                        ht->size--;
                        return 0; // Success
                }
                prev_entry = current_entry;
                current_entry = current_entry->next;
        }

        return -1; // Key not found
}

// resize
// Avoid the use of realloc here because you will end up reading and modifying
// the same table...
int resize(hash_table_t *ht) {
        // printf("resize initiated\n");
        int new_capacity_no = 2 * ht->capacity;
        entry_t **new_table =
            (entry_t **)malloc(new_capacity_no * sizeof(entry_t *));
        if (new_table == NULL) {
                printf("failed to allocate memory for new table: %s\n",
                       strerror(errno));
                return -1;
        }

        for (int i = 0; i < new_capacity_no; i++) {
                new_table[i] = NULL;
        }

        // Re-hash
        for (int i = 0; i < ht->capacity; i++) {
                entry_t *entry = ht->table[i];
                while (entry != NULL) {
                        entry_t *next = entry->next;
                        int address = hash(entry->key, new_capacity_no);
                        entry->next = new_table[address];
                        new_table[address] = entry;
                        entry = next;
                }
        }

        free(ht->table);
        ht->table = new_table;
        ht->capacity = new_capacity_no;

        // printf("resize finished\n");
        return 0;
}

// Keys
void print_keys(hash_table_t *ht) {
        printf("Keys:\n");
        for (int i = 0; i < ht->capacity; i++) {
                entry_t *entry = ht->table[i];
                if (entry == NULL) {
                        continue;
                }

                while (entry != NULL) {
                        printf("position: %d key: %s\n", i, entry->key);
                        entry = entry->next;
                }
        }
}

// CleanUp
void clean_up(hash_table_t *ht) {

        for (int i = 0; i < ht->capacity; i++) {
                entry_t *entry = ht->table[i];
                if (entry == NULL) {
                        continue;
                }
                while (entry != NULL) {
                        entry_t *tmp = entry->next;
                        free(entry->value);
                        free(entry->key);
                        free(entry);
                        entry = tmp;
                }
        }
        free(ht->table);
        free(ht);
}
