#ifndef HASHTABLE_H
#define HASHTABLE_H


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

// Entry struct
typedef struct Entry {
        char *Key;
        char *Value;
        struct Entry *Next;

} Entry;

// HashTable
typedef struct HashTable {
        int size;
        Entry **table;
} HashTable;

HashTable *createTable();
int Hash(char *key);
void Set(HashTable *ht, char *key, char *value);
char *Get(HashTable *ht, char *key);
void PrintKeys(HashTable *ht);
void CleanUp(HashTable *ht);

#endif
