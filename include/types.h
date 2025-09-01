#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Forward declaration for the Value struct
struct Value;

// Data type enumeration
typedef enum {
  TYPE_STRING,
  TYPE_INTEGER,
  TYPE_FLOAT,
  TYPE_BOOLEAN,
  TYPE_LIST,
  TYPE_HASH,
  TYPE_JSON,
} DataType;

// Linked list for LIST type
typedef struct ListNode {
  struct Value *value;
  struct ListNode *next;
} ListNode;

// Hash map entry for HASH type
typedef struct HashEntry {
  char *key;
  struct Value *value;
  struct HashEntry *next;
} HashEntry;

// Hash map for HASH type
typedef struct HashTable {
  HashEntry **buckets;
  size_t size;
} HashTable;

// The main Value struct
typedef struct Value {
  DataType type;
  union {
    char *string;
    int64_t integer;
    double floating_point;
    bool boolean;
    ListNode *list;
    HashTable *hash;
    char *json;
  } data;
} Value;

// TTL wrapper for values
typedef struct TTLValue {
  Value *value;
  uint64_t expiry; // 0 for no expiry
} TTLValue;

// Function prototypes for type creation and destruction
Value *create_string_value(const char *str);
Value *create_integer_value(int64_t i);
Value *create_float_value(double f);
Value *create_boolean_value(bool b);
Value *create_list_value();
Value *create_hash_value();
Value *create_json_value(const char *json_str);

void free_value(Value *value);

#endif // TYPES_H
