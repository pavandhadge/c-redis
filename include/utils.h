#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

// Get current time in milliseconds
uint64_t current_time_ms();

// Hash function for the store's hash table
unsigned long hash_string(const char *str);

#endif // UTILS_H
