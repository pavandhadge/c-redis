#define _POSIX_C_SOURCE 200809L
#include "../include/utils.h"
#include <time.h>
#include <time.h>

// Get current time in milliseconds
uint64_t current_time_ms() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return (uint64_t)spec.tv_sec * 1000 + (uint64_t)spec.tv_nsec / 1000000;
}

// FNV-1a hash function
unsigned long hash_string(const char* str) {
    unsigned long hash = 2166136261ul;
    for (const char* p = str; *p; p++) {
        hash ^= (unsigned long)(*p);
        hash *= 16777619ul;
    }
    return hash;
}
