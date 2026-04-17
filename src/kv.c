#include "kv.h"

#include <string.h>

kv_t *kv_init(const size_t capacity) {
    if (capacity <= 0) { return NULL; }

    kv_t *table = malloc(sizeof(kv_t));
    if (table == NULL) { return NULL; }

    table->capacity = capacity;
    table->count = 0;

    table->entries = calloc(sizeof(kv_entry_t), capacity);
    if (table->entries == NULL) { return NULL; }

    return table;
}

size_t hash(const char* val, const int capacity) {
    size_t hash = 0x13371337deadbeef;

    while (*val) {
        hash ^= *val;
        hash = hash << 8;
        hash += *val++;
    }

    return hash % capacity;
}

// function kv_put
// params:
//   - db: A pointer to the db
//   - key: A pointer to the key value
//   - value: A pointer to the value
// returns: The index of the key, on error -1, on not found -2
int kv_put(kv_t *db, const char *key, const char *value) {
    if (!db || !key || !value || db->capacity == 0) return -1;

    size_t idx = hash(key, db->capacity);

    for (int i = 0; i < db->capacity; i++) {
        size_t real_idx = (idx + i) % db->capacity;
        kv_entry_t *entry = &db->entries[real_idx];

        if (entry->key && 
            entry->key != TOMBSTONE && 
            strcmp(entry->key, key) == 0) {
            char *new_value = strdup(value);
            if (!new_value) 
                return -1;
            
            free(entry->value);
            entry->value = new_value;
            return 0;
        }

        if (!entry->key || entry->key == TOMBSTONE) {
            char *new_value = strdup(value);
            char *new_key = strdup(key);
            if (!new_value || !new_key) {
                free(new_value);
                free(new_key);
                return -1;
            }
            entry->key = new_key;
            entry->value = new_value;
            db->count++;
            return 0;
        }
    }

    return -2;
}


char *kv_get(kv_t *db, const char* key)
{
    if (!db || !key || db->capacity == 0) return NULL;

    size_t idx = hash(key, db->capacity);

    for (int i = 0; i < db->capacity; i++) {
        size_t real_idx = (idx + i) % db->capacity;
        kv_entry_t *entry = &db->entries[real_idx];

        if (!entry->key) {
            return NULL;
        }

        if (entry->key != TOMBSTONE && strcmp(entry->key, key) == 0) {
            return entry->value;
        }
    }

    return NULL;
}

int kv_delete(kv_t *db, const char* key) {
    if (!db || !key || db->capacity == 0) return -1;

    size_t idx = hash(key, db->capacity);

    for (int i = 0; i < db->capacity; i++) {
        size_t real_idx = (idx + i) % db->capacity;
        kv_entry_t *entry = &db->entries[real_idx];

        if (!entry->key) {
            return -1;
        }

        if (entry->key != TOMBSTONE && strcmp(entry->key, key) == 0) {
            free(entry->key);
            free(entry->value);
            entry->key = TOMBSTONE;
            entry->value = NULL;
            db->count--;
            return 0;
        }
    }

    return -2;
}