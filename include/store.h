#ifndef STORE_H
#define STORE_H

#include "types.h"
#include <stddef.h>

// The main data store structure (opaque)
typedef struct DataStore DataStore;

// --- Core Functions ---
DataStore *create_store();
void destroy_store(DataStore *store);

// --- Generic Operations ---
void set(DataStore *store, const char *key, Value *value, uint64_t ttl_ms);
Value *get(DataStore *store, const char *key);
void del(DataStore *store, const char *key);
bool exists(DataStore *store, const char *key);

// --- List Operations ---
bool list_push(DataStore *store, const char *key, Value *value);
Value *list_pop(DataStore *store, const char *key);
Value *list_peek(DataStore *store, const char *key);
size_t list_size(DataStore *store, const char *key);

// --- Hash Operations ---
bool hash_set(DataStore *store, const char *key, const char *field,
              Value *value);
Value *hash_get(DataStore *store, const char *key, const char *field);
bool hash_del(DataStore *store, const char *key, const char *field);

#endif // STORE_H
