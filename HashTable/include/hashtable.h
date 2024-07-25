#ifndef HASHTABLE_H
#define HASHTABLE_H


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Entry struct
typedef struct entry {
        char *key;
        char *value;
        struct entry *next;

} entry_t;

// HashTable
typedef struct hash_table {
        int capacity;
        int size;
        entry_t **table;
} hash_table_t;

hash_table_t *create_table(int capacity);
int hash(char *key, int capacity);
int set_value(hash_table_t *ht, char *key, char *value);
char *get_value(hash_table_t *ht, char *key);
void print_keys(hash_table_t *ht);
void clean_up(hash_table_t *ht);
int delete_entry(hash_table_t *ht, char *key);
int resize(hash_table_t *ht);

#endif
