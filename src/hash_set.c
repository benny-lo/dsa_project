#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_set.h"

#define LOAD_FACTOR 0.6
#define GROWTH_FACTOR 1.5


size_t hash(const hash_set_t * hash_set, const char * key) {
    size_t x = (size_t) key;
    return (x * 2654435761U) % (hash_set->capacity);
}

void hash_set_resize(hash_set_t * hash_set, size_t game_id) {
    size_t new_capacity = hash_set->capacity * GROWTH_FACTOR;

    key_value_t * new_table = (key_value_t *) malloc(new_capacity * sizeof(key_value_t));
    for (size_t i = 0; i < new_capacity; i++) {
        new_table[i].address = NULL;
        new_table[i].game_id = 0;
    }

    memcpy(new_table, hash_set->table, hash_set->capacity * sizeof(key_value_t));

    free(hash_set->table);
    hash_set->table = new_table;

    //fprintf(stderr, "Resized hash set from %zu to %zu\n", hash_set->capacity, new_capacity);
    hash_set->capacity = new_capacity;
}

hash_set_t * hash_set_create(size_t capacity) {
    hash_set_t * hash_set = (hash_set_t *) malloc(sizeof(hash_set_t));
    if (hash_set == NULL) {
        fprintf(stderr, "Failed to allocate memory for hash set\n");
        return NULL;
    }

    hash_set->table = (key_value_t *) malloc(capacity * sizeof(key_value_t));
    if (hash_set->table == NULL) {
        fprintf(stderr, "Failed to allocate memory for hash set table\n");
        free(hash_set);
        return NULL;
    }

    for(size_t i = 0; i < capacity; i++) {
        hash_set->table[i].address = NULL;
        hash_set->table[i].game_id = 0;
    }

    hash_set->size = 0;
    hash_set->capacity = capacity;
    return hash_set;
}


void hash_set_destroy(hash_set_t * hash_set) {
    free(hash_set->table);
    free(hash_set);
}


void hash_set_insert(hash_set_t * hash_set, const char * address, size_t game_id) {
    size_t index = hash(hash_set, address);
    size_t original_index = index;

    size_t counter = 0;

    while (hash_set->table[index].address != NULL && hash_set->table[index].address != address) {
        index = (index + 1) % hash_set->capacity;
        counter++;
        if (counter > hash_set->capacity) {
            fprintf(stderr, "Hash set is full\n");
            return;
        }
    }

    if (hash_set->table[index].address == NULL) {
        hash_set->size++;
        hash_set->table[index].address = address;
    }

    hash_set->table[index].game_id = game_id;

    if (hash_set->size > hash_set->capacity * LOAD_FACTOR) {
        hash_set_resize(hash_set, game_id);
    }
}


bool hash_set_get(hash_set_t * hash_set, const char * address, size_t * game_id) {
    size_t index = hash(hash_set, address);
    size_t original_index = index;

    //fprintf(stderr, "Getting %s at index %zu\n", address, index);
    //fprintf(stderr, "Hash set capacity: %zu\n", hash_set->capacity);

    size_t counter = 0;

    while (hash_set->table[index].address != NULL && hash_set->table[index].address != address) {
        index = (index + 1) % hash_set->capacity;
        counter++;
        if (counter > hash_set->capacity) {
            return false;
        }
    }
    
    if (hash_set->table[index].address == NULL) {
        return false;
    }

    *game_id = hash_set->table[index].game_id;
    return true;
}


void hash_set_remove(hash_set_t * hash_set, const char * address) {
    size_t index = hash(hash_set, address);
    size_t original_index = index;

    size_t counter = 0;

    while (hash_set->table[index].address != NULL && hash_set->table[index].address != address) {
        index = (index + 1) % hash_set->capacity;
        counter++;
        if (counter > hash_set->capacity) {
            return;
        }
    }

    if (hash_set->table[index].address == NULL) {
        return;
    }

    hash_set->table[index].game_id = 0;
}