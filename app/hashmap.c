#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TABLE_SIZE 100

typedef struct KeyValuePair {
    char key[100];
    int value;
    struct KeyValuePair *next;
} KeyValuePair;

typedef struct {
    KeyValuePair *buckets[TABLE_SIZE];
} HashTable;

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

int main() {
    HashTable ht = {0};  // Initialize all pointers to NULL

    insert(&ht, "one", 1);
    insert(&ht, "two", 2);
    insert(&ht, "three", 3);

    printf("one: %d\n", get(&ht, "one"));
    printf("two: %d\n", get(&ht, "two"));
    printf("four: %d\n", get(&ht, "four")); // Will print -1 as "four" isn't in our hashtable

    // Don't forget to free memory before exiting!
    // ... 

    return 0;
}

