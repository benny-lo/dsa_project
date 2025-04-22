#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stdlib.h>

extern const char OUT_OF_BOUNDS;

typedef struct memory_pool_t {
    size_t capacity; // number of pages in the pool.

    size_t size; // number of allocated pages - 1. the page with index size is the last allocated page
    size_t page_offset; // offset of the current page

    char ** memory; // pages
} memory_pool_t; 

// creates a new memory pool with the given initial capacity
memory_pool_t* memory_pool_create(size_t initial_capacity);

// destroys the memory pool
void memory_pool_destroy(memory_pool_t* pool);

// requests enough space in the pool to store the request string.
// returns true if the request was successful, false otherwise.
// if successful, the base address of the requested string is stored in base.
bool memory_pool_request(memory_pool_t* pool, const char * request, size_t request_size, size_t * base);

// accesses the character at the address base + offset
char memory_pool_access(const memory_pool_t* pool, size_t base, size_t offset);

#endif
