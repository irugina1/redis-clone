#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>


#define TABLE_SIZE 100

typedef struct KeyValuePair {
    char key[100];
    char value[100];
    struct timeval tv;
    struct KeyValuePair *next;
} KeyValuePair;

typedef struct {
    KeyValuePair *buckets[TABLE_SIZE];
    pthread_mutex_t mutex; // lock
} HashTable;

// Function prototypes
unsigned int hash(const char *key);
void insert(HashTable *ht, const char *key, const char *value, const int expiry_ms);
char *get(HashTable *ht, const char *key);
void initializeHashTable(HashTable *ht);
void freeHashTable(HashTable *ht);

#endif // HASH_TABLE_H
