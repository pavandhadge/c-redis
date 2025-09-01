#include <stdio.h>
#include <unistd.h>
#include "../include/store.h"

void print_value(Value* v) {
    if (!v) {
        printf("(nil)\n");
        return;
    }
    switch (v->type) {
        case TYPE_STRING:
            printf("\"%s\"\n", v->data.string);
            break;
        case TYPE_INTEGER:
            printf("%ld\n", v->data.integer);
            break;
        case TYPE_FLOAT:
            printf("%f\n", v->data.floating_point);
            break;
        case TYPE_BOOLEAN:
            printf("%s\n", v->data.boolean ? "true" : "false");
            break;
        case TYPE_JSON:
            printf("%s\n", v->data.json);
            break;
        default:
            printf("Complex type\n");
            break;
    }
}

int main() {
    DataStore* store = create_store();

    printf("--- Basic Types ---\n");
    set(store, "mykey", create_string_value("Hello, World!"), 0);
    Value* mykey_val = get(store, "mykey");
    printf("mykey: ");
    print_value(mykey_val);

    set(store, "mynumber", create_integer_value(123), 0);
    Value* mynumber_val = get(store, "mynumber");
    printf("mynumber: ");
    print_value(mynumber_val);

    printf("\n--- TTL ---\n");
    set(store, "ttl_key", create_string_value("I will expire"), 2000);
    printf("ttl_key (before expiry): ");
    print_value(get(store, "ttl_key"));
    sleep(3);
    printf("ttl_key (after expiry): ");
    print_value(get(store, "ttl_key"));

    printf("\n--- List ---\n");
    list_push(store, "mylist", create_string_value("one"));
    list_push(store, "mylist", create_string_value("two"));
    printf("mylist size: %zu\n", list_size(store, "mylist"));
    Value* popped = list_pop(store, "mylist");
    printf("Popped: ");
    print_value(popped);
    free_value(popped);
    printf("mylist size after pop: %zu\n", list_size(store, "mylist"));

    printf("\n--- Hash ---\n");
    hash_set(store, "myhash", "field1", create_string_value("value1"));
    hash_set(store, "myhash", "field2", create_integer_value(42));
    printf("myhash.field1: ");
    print_value(hash_get(store, "myhash", "field1"));
    printf("myhash.field2: ");
    print_value(hash_get(store, "myhash", "field2"));

    printf("\n--- JSON ---\n");
    const char* json = "{\"name\": \"John Doe\", \"age\": 30}";
    set(store, "myjson", create_json_value(json), 0);
    printf("myjson: ");
    print_value(get(store, "myjson"));

    destroy_store(store);
    return 0;
}
