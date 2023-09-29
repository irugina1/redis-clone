#include "hashmap.h"

unsigned int hash(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + *key++;  // Simple hash function
    }
    return hash % TABLE_SIZE;
}

void insert(HashTable *ht, const char *key, int value) {
    pthread_mutex_lock(&ht->mutex);  // Lock the mutex
    unsigned int idx = hash(key);
    KeyValuePair *new_kv = malloc(sizeof(KeyValuePair));
    strcpy(new_kv->key, key);
    new_kv->value = value;
    new_kv->next = NULL;

    if (!ht->buckets[idx]) {
        ht->buckets[idx] = new_kv;
    } else {
        KeyValuePair *current = ht->buckets[idx];
        while (current->next) {
            if (strcmp(current->key, key) == 0) {
                current->value = value;
                free(new_kv);
                pthread_mutex_unlock(&ht->mutex);  // Unlock the mutex
                return;
            }
            current = current->next;
        }
        current->next = new_kv;
    }
    pthread_mutex_unlock(&ht->mutex);  // Unlock the mutex
}

int get(HashTable *ht, const char *key) {
    pthread_mutex_lock(&ht->mutex);  // Lock the mutex
    unsigned int idx = hash(key);
    KeyValuePair *current = ht->buckets[idx];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            pthread_mutex_unlock(&ht->mutex);  // Unlock the mutex
            return current->value;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&ht->mutex);  // Unlock the mutex
    return -1;  // Sentinel value indicating key was not found
}

void initializeHashTable(HashTable *ht) {
    memset(ht->buckets, 0, sizeof(ht->buckets));  // Initialize all pointers to NULL
    pthread_mutex_init(&ht->mutex, NULL);  // Initialize the mutex
}

void freeHashTable(HashTable *ht) {
    //TODO free key-value pairs
    pthread_mutex_destroy(&ht->mutex);  // Destroy the mutex
}
