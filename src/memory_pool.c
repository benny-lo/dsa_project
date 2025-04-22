#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "memory_pool.h"

#define GROWTH_FACTOR 1.6
#define ALLOC_SIZE 16

const char OUT_OF_BOUNDS = '=';


void memory_pool_grow(memory_pool_t * pool, size_t new_capacity);
void memory_pool_copy(memory_pool_t * pool, const char * request, size_t request_size, size_t * copied);

memory_pool_t* memory_pool_create(size_t initial_capacity) {
    if (initial_capacity == 0) {
        fprintf(stderr, "initial_capacity is 0 in memory_pool_create\n");
        return NULL;
    }

    memory_pool_t* pool = (memory_pool_t*) malloc(sizeof(memory_pool_t));
    if (pool == NULL) {
        fprintf(stderr, "failed allocation of memory_pool_t in memory_pool_create\n");
        return NULL;
    }

    pool->capacity = initial_capacity;

    pool->size = 0;
    pool->page_offset = 0;

    pool->memory = (char**) malloc(sizeof(char*) * initial_capacity);
    if (pool->memory == NULL) {
        fprintf(stderr, "failed allocation of memory_pool_t in memory_pool_create\n");
        free(pool);
        return NULL;
    }

    for(size_t i = 0; i < initial_capacity; i++) {
        pool->memory[i] = NULL;
    }

    pool->memory[0] = (char*) malloc(sizeof(char) * ALLOC_SIZE);
    if (pool->memory[0] == NULL) {
        fprintf(stderr, "failed allocation of memory_pool_t in memory_pool_create\n");
        free(pool->memory);
        free(pool);
        return NULL;
    }

    return pool;
}


void memory_pool_destroy(memory_pool_t* pool) {
    for(size_t i = 0; i < pool->capacity; i++) {
        if (pool->memory[i] != NULL) {
            free(pool->memory[i]);
        }
    }
    free(pool->memory);
    free(pool);
}


bool memory_pool_request(memory_pool_t* pool, const char * request, size_t request_size, size_t * base) {
    size_t real_size = ALLOC_SIZE * (pool->size) + pool->page_offset;
    size_t real_capacity = ALLOC_SIZE * (pool->capacity);

    if (request_size > real_capacity - real_size) {
        if (pool->capacity > SIZE_MAX / GROWTH_FACTOR || real_size > SIZE_MAX - request_size) {
            fprintf(stderr, "memory_pool_request: capacity overflow\n");
            return false;
        }

        size_t new_capacity = pool->capacity * GROWTH_FACTOR;
        if (new_capacity < real_size + request_size) {
            new_capacity = real_size + request_size;
        }

        memory_pool_grow(pool, new_capacity);
    }

    *base = ALLOC_SIZE * (pool->size) + pool->page_offset;

    size_t copied = 0;
    do {
        size_t tmp;
        memory_pool_copy(pool, request + copied, request_size - copied, &tmp);
        
        copied += tmp;

        if (pool->size + 1 >= pool->capacity) {
            memory_pool_grow(pool, pool->capacity * GROWTH_FACTOR);
        }
        pool->size++;
        pool->page_offset = 0;

        pool->memory[pool->size] = (char*) malloc(sizeof(char) * ALLOC_SIZE);
        if (pool->memory[pool->size] == NULL) {
            fprintf(stderr, "failed allocation of memory_pool_t in memory_pool_request\n");
            return false;
        }
    } while (copied < request_size);

    return true;
}


char memory_pool_access(const memory_pool_t* pool, size_t base, size_t offset) {
    size_t page_offset = (base + offset) % ALLOC_SIZE;
    size_t pages = (base + offset) / ALLOC_SIZE;

    if (pages >= pool->capacity) {
        fprintf(stderr, "memory_pool_access: out of bounds\n");
        return OUT_OF_BOUNDS;
    }

    return pool->memory[pages][page_offset];
}


// Private functions

void memory_pool_grow(memory_pool_t * pool, size_t new_capacity) {
    char ** new_memory = (char**) malloc(sizeof(char*) * new_capacity);

    memcpy(new_memory, pool->memory, sizeof(char*) * (pool->size + 1));

    for(size_t i = pool->size + 1; i < new_capacity; i++) {
        new_memory[i] = NULL;
    }

    free(pool->memory);
    pool->memory = new_memory;
    pool->capacity = new_capacity;
}


void memory_pool_copy(memory_pool_t * pool, const char * request, size_t request_size, size_t * copied) {
    size_t available = ALLOC_SIZE - pool->page_offset;
    *copied = available;
    
    if (request_size <= available) {
        *copied = request_size;
    }
    
    memcpy(pool->memory[pool->size], request, *copied);
    pool->page_offset += *copied;
}
