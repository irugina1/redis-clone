#include "hashmap.h"

unsigned int hash(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + *key++;  // Simple hash function
    }
    return hash % TABLE_SIZE;
}

void add_time_offset(struct timeval *tv, long offset_ms) {
    // Convert offset into seconds and microseconds
    long sec_offset = offset_ms / 1000;
    long usec_offset = (offset_ms % 1000) * 1000;

    tv->tv_sec += sec_offset;
    tv->tv_usec += usec_offset;

    // Handle overflow, if microseconds exceed 1,000,000
    if (tv->tv_usec >= 1000000) {
        tv->tv_sec += 1;
        tv->tv_usec -= 1000000;
    }
}

/*
If the result is -1, tv1 is less than tv2.
If the result is 1, tv1 is greater than tv2.
If the result is 0, tv1 is equal to tv2.
*/
int timeval_compare(const struct timeval *tv1, const struct timeval *tv2) {
    if (tv1->tv_sec < tv2->tv_sec) return -1;
    if (tv1->tv_sec > tv2->tv_sec) return 1;
    if (tv1->tv_usec < tv2->tv_usec) return -1;
    if (tv1->tv_usec > tv2->tv_usec) return 1;
    return 0;  // they are equal
}

void insert(HashTable *ht, const char *key, const char* value, const int expiry_ms) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("inserting at = %ld.%06d\n", tv.tv_sec, tv.tv_usec);
    if (expiry_ms > 0) {
        // get current time and compute timeval tv (in the future) when key expires
        add_time_offset(&tv, expiry_ms);
    }
    else {
        tv.tv_sec = -1;
        tv.tv_usec = 0;
    }
    printf("will expire at = %ld.%06d\n", tv.tv_sec, tv.tv_usec);

    pthread_mutex_lock(&ht->mutex);  // Lock the mutex
    unsigned int idx = hash(key);
    KeyValuePair *new_kv = malloc(sizeof(KeyValuePair));
    strcpy(new_kv->key, key);
    strcpy(new_kv->value, value);
    new_kv->tv = tv;
    new_kv->next = NULL;

    if (!ht->buckets[idx]) {
        ht->buckets[idx] = new_kv;
    } else {
        KeyValuePair *current = ht->buckets[idx];
        while (current->next) {
            if (strcmp(current->key, key) == 0) {
                strcpy(current->value, value);
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

char* get(HashTable *ht, const char *key) {
    pthread_mutex_lock(&ht->mutex);  // Lock the mutex
    unsigned int idx = hash(key);
    KeyValuePair *current = ht->buckets[idx];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            struct timeval expiry_tv = current->tv;
            if (expiry_tv.tv_sec >= 0){ // check if it expired or no
                // time right now
                struct timeval current_tv;
                gettimeofday(&current_tv, NULL);
                // print the two timestamps
                printf("expiry time = %ld.%06d\n", expiry_tv.tv_sec, expiry_tv.tv_usec);
                printf("current time = %ld.%06d\n", current_tv.tv_sec, current_tv.tv_usec);
                // compare expiry with current time
                int timeval_comparison = timeval_compare(&expiry_tv ,&current_tv);
                printf("timeval comparison = %d\n", timeval_comparison);
                if (timeval_comparison < 0){
                    current = NULL;
                    return NULL;
                }
	    }
            pthread_mutex_unlock(&ht->mutex);  // Unlock the mutex
            return current->value;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&ht->mutex);  // Unlock the mutex
    return NULL;  // Sentinel value indicating key was not found
}

void initializeHashTable(HashTable *ht) {
    memset(ht->buckets, 0, sizeof(ht->buckets));  // Initialize all pointers to NULL
    pthread_mutex_init(&ht->mutex, NULL);  // Initialize the mutex
}

void freeHashTable(HashTable *ht) {
    //TODO free key-value pairs
    pthread_mutex_destroy(&ht->mutex);  // Destroy the mutex
}
