#include "../include/hashtable.h"

// init table
HashTable *createTable(int capacity) {
        HashTable *ht = malloc(sizeof(HashTable));
        if (!ht) {
                printf("failed to allocate memory: %s", strerror(errno));
                exit(-1);
        }

        ht->table = malloc(sizeof(Entry *) * capacity);
        if (!ht->table) {
                printf("failed to allocate memory: %s", strerror(errno));
                free(ht);
                exit(-1);
        }

        ht->capacity = capacity;

        for (int i = 0; i < capacity; i++) {
                ht->table[i] = NULL;
        }
        // ht->size = size;

        return ht;
}

// Hash
int Hash(char *key, int capacity) {
        unsigned int hash = 0;
        while (*key != '\0') {
                hash = (hash << 5) + *key++;
        }

        // printf("address: %d\n", hash% TABLE_SIZE);

        return hash % capacity;
}

// Set
void Set(HashTable *ht, char *key, char *value) {
        int address = Hash(key, ht->capacity);

        Entry *entry = ht->table[address];

        while (entry != NULL) {
                if (strcmp(entry->Key, key) == 0) {
                        // printf("entry key: %s key: %s\n", entry->Key, key);
                        free(entry->Value);
                        entry->Value = strdup(value);
                        if (!entry->Value) {
                                printf("failed to allocate memory: %s",
                                       strerror(errno));
                                return;
                        }
                        return;
                }
                entry = entry->Next;
        }

        entry = malloc(sizeof(Entry)); // consider also calloc
        if (!entry) {
                printf("failed to allocate memory: %s", strerror(errno));
                return;
                // exit(-1);
        }
        entry->Key = strdup(key);
        entry->Value = strdup(value);
        if (!entry->Key || !entry->Value) {
                printf("failed to allocate memory for key or value: %s\n",
                       strerror(errno));
                free(entry->Key);
                free(entry->Value);
                free(entry);
                return;
        }

        // if(ht->table[address] == NULL){
        // 	entry->Next = NULL;
        // 	ht->table[address] = entry;
        //
        // 	return;
        // }

        entry->Next = ht->table[address];
        ht->table[address] = entry;
}

// Get
char *Get(HashTable *ht, char *key) {
        int address = Hash(key, ht->capacity);
        // printf("address: %d\n", address);
        // printf("get key: %s\n",key);

        Entry *entry = ht->table[address];
        // printf("get entry: %p\n",entry);

        // if( entry == NULL) {
        // 	return NULL;
        // }

        while (entry != NULL) {
                if (strcmp(entry->Key, key) == 0) {
                        return entry->Value;
                }
                entry = entry->Next;
        }

        return NULL;
}

// Keys
void PrintKeys(HashTable *ht) {
        printf("Keys:\n");
        for (int i = 0; i < ht->capacity; i++) {
                Entry *entry = ht->table[i];
                if (entry == NULL) {
                        continue;
                }

                while (entry != NULL) {
                        printf("%s\n", entry->Key);
                        entry = entry->Next;
                }
        }
}

// CleanUp
void CleanUp(HashTable *ht) {

        for (int i = 0; i < ht->capacity; i++) {
                Entry *entry = ht->table[i];
                if (entry == NULL) {
                        continue;
                }
                while (entry != NULL) {
                        Entry *tmp = entry->Next;
                        free(entry->Value);
                        free(entry->Key);
                        free(entry);
                        entry = tmp;
                }
        }
        free(ht->table);
        free(ht);
}

// void delete(char *key) {
//         printf("Needs Implementation");
//         exit(-1);
// }
