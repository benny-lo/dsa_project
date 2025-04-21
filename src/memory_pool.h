#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stdlib.h>

extern const char OUT_OF_BOUNDS;

typedef struct memory_pool_t {
    size_t capacity;

    size_t size;
    size_t page_offset; 

    char ** memory;
} memory_pool_t; 

memory_pool_t* memory_pool_create(size_t initial_capacity);
void memory_pool_destroy(memory_pool_t* pool);

bool memory_pool_request(memory_pool_t* pool, const char * request, size_t request_size, size_t * memory_id, size_t * lower_bound);
char memory_pool_access(const memory_pool_t* pool, size_t memory_id, size_t offset);

#endif
