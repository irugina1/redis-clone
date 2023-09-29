#include "hash_table.h"

unsigned int hash(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + *key++;  // Simple hash function
    }
    return hash % TABLE_SIZE;
}

void insert(HashTable *ht, const char *key, int value) {
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
                return;
            }
            current = current->next;
        }
        current->next = new_kv;
    }
}

int get(HashTable *ht, const char *key) {
    unsigned int idx = hash(key);
    KeyValuePair *current = ht->buckets[idx];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return -1;  // Sentinel value indicating key was not found
}
