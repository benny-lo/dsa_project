#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"
#include "rax.h"
#include "hash_set.h"
/*
 * Actual constraint bookkeeping: 
 * for every position, the character forced in that position ('+');
 * for every character, lower bound on the number of occurences in the string ('+' and '|');
 * for every character, the set of positions where it cannot appear ('|' and '/').
 */



/*
 * Structure to store constraints and filtering information for word matching.
 * 
 * This structure maintains information about character occurrences and constraints
 * used to filter valid words in the dictionary.
 *
 * occur:   Array tracking required minimum occurrences for each character
 * option:  Array indicating if a character must match exact occurrence count
 * forced:  Array storing required characters at specific positions
 * appear:  Matrix storing allowed characters at each position
 */
typedef struct help_t {
    size_t occur[ALPHABET_SIZE]; 
    bool option[ALPHABET_SIZE]; 
    char* forced; 
    bool* appear; 
} help_t; 

help_t* help_alloc(size_t); 
void help_reset(help_t*, size_t); 
void help_dealloc(help_t*); 


// Converts a character to its corresponding index in the alphabet (just made up of 64 possible characters)
// Returns -1 if character is invalid
int char_index(char c); 

void reset(char *, int *, int); 
void substring_copy(char *, const char *, size_t, size_t); 
void gen_constraint(char *, char *, char *, help_t *, int); 
bool compatible(const char *, const help_t *, size_t);

int better_update_filter(rax_t *, int [], int, help_t *, int, hash_set_t *); 

#endif 