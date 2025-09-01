#define _POSIX_C_SOURCE 200809L
#include "../include/types.h"
#include <stdlib.h>
#include <string.h>

Value* create_string_value(const char* str) {
    Value* v = (Value*)malloc(sizeof(Value));
    v->type = TYPE_STRING;
    v->data.string = strdup(str);
    return v;
}

Value* create_integer_value(int64_t i) {
    Value* v = (Value*)malloc(sizeof(Value));
    v->type = TYPE_INTEGER;
    v->data.integer = i;
    return v;
}

Value* create_float_value(double f) {
    Value* v = (Value*)malloc(sizeof(Value));
    v->type = TYPE_FLOAT;
    v->data.floating_point = f;
    return v;
}

Value* create_boolean_value(bool b) {
    Value* v = (Value*)malloc(sizeof(Value));
    v->type = TYPE_BOOLEAN;
    v->data.boolean = b;
    return v;
}

Value* create_list_value() {
    Value* v = (Value*)malloc(sizeof(Value));
    v->type = TYPE_LIST;
    v->data.list = NULL;
    return v;
}

Value* create_hash_value() {
    Value* v = (Value*)malloc(sizeof(Value));
    v->type = TYPE_HASH;
    v->data.hash = (HashTable*)calloc(1, sizeof(HashTable));
    v->data.hash->size = 16; // Initial size
    v->data.hash->buckets = (HashEntry**)calloc(v->data.hash->size, sizeof(HashEntry*));
    return v;
}

Value* create_json_value(const char* json_str) {
    Value* v = (Value*)malloc(sizeof(Value));
    v->type = TYPE_JSON;
    v->data.json = strdup(json_str);
    return v;
}

void free_value(Value* value) {
    if (!value) return;

    switch (value->type) {
        case TYPE_STRING:
            free(value->data.string);
            break;
        case TYPE_JSON:
            free(value->data.json);
            break;
        case TYPE_LIST: {
            ListNode* current = value->data.list;
            while (current) {
                ListNode* next = current->next;
                free_value(current->value);
                free(current);
                current = next;
            }
            break;
        }
        case TYPE_HASH: {
            HashTable* ht = value->data.hash;
            for (size_t i = 0; i < ht->size; i++) {
                HashEntry* entry = ht->buckets[i];
                while (entry) {
                    HashEntry* next = entry->next;
                    free(entry->key);
                    free_value(entry->value);
                    free(entry);
                    entry = next;
                }
            }
            free(ht->buckets);
            free(ht);
            break;
        }
        default:
            // No dynamic allocation for other types
            break;
    }
    free(value);
}
