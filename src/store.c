#define _POSIX_C_SOURCE 200809L
#include "../include/store.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <string.h>
#include <string.h>

#define INITIAL_STORE_SIZE 1024

// Internal structure for a key-value pair in the store
typedef struct StoreEntry {
    char* key;
    TTLValue* ttl_value;
    struct StoreEntry* next;
} StoreEntry;

// The main data store structure
struct DataStore {
    StoreEntry** buckets;
    size_t size;
    size_t count;
};

DataStore* create_store() {
    DataStore* store = (DataStore*)malloc(sizeof(DataStore));
    store->size = INITIAL_STORE_SIZE;
    store->count = 0;
    store->buckets = (StoreEntry**)calloc(store->size, sizeof(StoreEntry*));
    return store;
}

void destroy_store(DataStore* store) {
    if (!store) return;
    for (size_t i = 0; i < store->size; i++) {
        StoreEntry* entry = store->buckets[i];
        while (entry) {
            StoreEntry* next = entry->next;
            free(entry->key);
            free_value(entry->ttl_value->value);
            free(entry->ttl_value);
            free(entry);
            entry = next;
        }
    }
    free(store->buckets);
    free(store);
}

void set(DataStore* store, const char* key, Value* value, uint64_t ttl_ms) {
    unsigned long hash = hash_string(key);
    size_t index = hash % store->size;
    StoreEntry* entry = store->buckets[index];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            free_value(entry->ttl_value->value);
            entry->ttl_value->value = value;
            entry->ttl_value->expiry = (ttl_ms > 0) ? current_time_ms() + ttl_ms : 0;
            return;
        }
        entry = entry->next;
    }

    StoreEntry* new_entry = (StoreEntry*)malloc(sizeof(StoreEntry));
    new_entry->key = strdup(key);
    new_entry->ttl_value = (TTLValue*)malloc(sizeof(TTLValue));
    new_entry->ttl_value->value = value;
    new_entry->ttl_value->expiry = (ttl_ms > 0) ? current_time_ms() + ttl_ms : 0;
    new_entry->next = store->buckets[index];
    store->buckets[index] = new_entry;
    store->count++;
}

Value* get(DataStore* store, const char* key) {
    unsigned long hash = hash_string(key);
    size_t index = hash % store->size;
    StoreEntry* entry = store->buckets[index];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (entry->ttl_value->expiry > 0 && entry->ttl_value->expiry < current_time_ms()) {
                del(store, key);
                return NULL;
            }
            return entry->ttl_value->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void del(DataStore* store, const char* key) {
    unsigned long hash = hash_string(key);
    size_t index = hash % store->size;
    StoreEntry* entry = store->buckets[index];
    StoreEntry* prev = NULL;

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                store->buckets[index] = entry->next;
            }
            free(entry->key);
            free_value(entry->ttl_value->value);
            free(entry->ttl_value);
            free(entry);
            store->count--;
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

bool exists(DataStore* store, const char* key) {
    return get(store, key) != NULL;
}

bool list_push(DataStore* store, const char* key, Value* value) {
    Value* list_val = get(store, key);
    if (!list_val) {
        list_val = create_list_value();
        set(store, key, list_val, 0);
    } else if (list_val->type != TYPE_LIST) {
        return false; // Key exists but is not a list
    }

    ListNode* new_node = (ListNode*)malloc(sizeof(ListNode));
    new_node->value = value;
    new_node->next = list_val->data.list;
    list_val->data.list = new_node;
    return true;
}

Value* list_pop(DataStore* store, const char* key) {
    Value* list_val = get(store, key);
    if (!list_val || list_val->type != TYPE_LIST || !list_val->data.list) {
        return NULL;
    }

    ListNode* top_node = list_val->data.list;
    Value* val = top_node->value;
    list_val->data.list = top_node->next;
    free(top_node);
    return val;
}

Value* list_peek(DataStore* store, const char* key) {
    Value* list_val = get(store, key);
    if (!list_val || list_val->type != TYPE_LIST || !list_val->data.list) {
        return NULL;
    }
    return list_val->data.list->value;
}

size_t list_size(DataStore* store, const char* key) {
    Value* list_val = get(store, key);
    if (!list_val || list_val->type != TYPE_LIST) {
        return 0;
    }

    size_t count = 0;
    ListNode* current = list_val->data.list;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

bool hash_set(DataStore* store, const char* key, const char* field, Value* value) {
    Value* hash_val = get(store, key);
    if (!hash_val) {
        hash_val = create_hash_value();
        set(store, key, hash_val, 0);
    } else if (hash_val->type != TYPE_HASH) {
        return false; // Key exists but is not a hash
    }

    HashTable* ht = hash_val->data.hash;
    unsigned long hash = hash_string(field);
    size_t index = hash % ht->size;
    HashEntry* entry = ht->buckets[index];

    while (entry) {
        if (strcmp(entry->key, field) == 0) {
            free_value(entry->value);
            entry->value = value;
            return true;
        }
        entry = entry->next;
    }

    HashEntry* new_entry = (HashEntry*)malloc(sizeof(HashEntry));
    new_entry->key = strdup(field);
    new_entry->value = value;
    new_entry->next = ht->buckets[index];
    ht->buckets[index] = new_entry;
    return true;
}

Value* hash_get(DataStore* store, const char* key, const char* field) {
    Value* hash_val = get(store, key);
    if (!hash_val || hash_val->type != TYPE_HASH) {
        return NULL;
    }

    HashTable* ht = hash_val->data.hash;
    unsigned long hash = hash_string(field);
    size_t index = hash % ht->size;
    HashEntry* entry = ht->buckets[index];

    while (entry) {
        if (strcmp(entry->key, field) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

bool hash_del(DataStore* store, const char* key, const char* field) {
    Value* hash_val = get(store, key);
    if (!hash_val || hash_val->type != TYPE_HASH) {
        return false;
    }

    HashTable* ht = hash_val->data.hash;
    unsigned long hash = hash_string(field);
    size_t index = hash % ht->size;
    HashEntry* entry = ht->buckets[index];
    HashEntry* prev = NULL;

    while (entry) {
        if (strcmp(entry->key, field) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                ht->buckets[index] = entry->next;
            }
            free(entry->key);
            free_value(entry->value);
            free(entry);
            return true;
        }
        prev = entry;
        entry = entry->next;
    }
    return false;
}
