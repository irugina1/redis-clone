#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <phtread.h>

#define TABLE_SIZE 100

typedef struct KeyValuePair {
    char key[100];
    int value;
    struct KeyValuePair *next;
} KeyValuePair;

typedef struct {
    KeyValuePair *buckets[TABLE_SIZE];
    pthread_mutex_t mutex; // lock
} HashTable;

// Function prototypes
unsigned int hash(const char *key);
void insert(HashTable *ht, const char *key, int value);
int get(HashTable *ht, const char *key);

#endif // HASH_TABLE_H
