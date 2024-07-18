#ifndef HASHTABLE_H
#define HASHTABLE_H


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Entry struct
typedef struct Entry {
        char *Key;
        char *Value;
        struct Entry *Next;

} Entry;

// HashTable
typedef struct HashTable {
        int capacity;
        Entry **table;
} HashTable;

HashTable *createTable(int capacity);
int Hash(char *key, int capacity);
void Set(HashTable *ht, char *key, char *value);
char *Get(HashTable *ht, char *key);
void PrintKeys(HashTable *ht);
void CleanUp(HashTable *ht);

#endif
