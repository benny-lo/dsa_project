#ifndef HASH_SET_H
#define HASH_SET_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct key_value_t {
    const char * address;
    size_t game_id;
} key_value_t; 

typedef struct hash_set_t {
    size_t size; 
    size_t capacity; 
    key_value_t * table; 
} hash_set_t; 


hash_set_t * hash_set_create(size_t capacity); 
void hash_set_destroy(hash_set_t * hash_set); 

void hash_set_insert(hash_set_t * hash_set, const char * address, size_t game_id); 
bool hash_set_get(hash_set_t * hash_set, const char * address, size_t * game_id); 
void hash_set_remove(hash_set_t * hash_set, const char * address); 

#endif

