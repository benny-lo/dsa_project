#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"
#include "rax.h"

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
    int occur[ALPHABET_SIZE]; 
    bool option[ALPHABET_SIZE]; 
    char* forced; 
    bool* appear; 
} help_t; 

/*
 * Allocates a new help_t structure for managing constraints.
 * Parameters:
 * - size_t k: Size of the string for which constraints are being managed
 */
help_t* help_alloc(size_t k); 

/*
 * Resets the help_t structure to its initial state.
 * Parameters:
 * - help_t* info: Pointer to the help_t structure to reset
 * - size_t k: Size of the string for which constraints are being managed
 */
void help_reset(help_t* info, size_t k); 

/*
 * Deallocates a help_t structure and its associated memory.
 * Parameters:
 * - help_t* info: Pointer to the help_t structure to deallocate
 */
void help_dealloc(help_t* info); 

/*
 * Copies a substring from source to destination.
 * Parameters:
 * - char* dest: Destination string where the substring will be copied
 * - const char* source: Source string from which the substring will be copied
 * - size_t a: Starting index in the source string
 * - size_t b: Ending index in the source string
 */
void substring_copy(char *dest, const char * source, size_t a, size_t b); 

/*
 * Generates constraints based on the reference and guess strings.
 * Updates the help_t structure with the new constraints.
 * Parameters:
 * - const char* ref: Reference string
 * - const char* guess: Guess string
 * - char* constraint: String to store the generated constraints
 * - help_t* info: Pointer to the help_t structure to update
 * - int k: Length of the strings
 */
void gen_constraint(const char * ref, const char * guess, char * constraint, help_t * info, size_t k); 

/*
 * Checks if a given string is compatible with the constraints stored in the help_t structure.
 * Parameters:
 * - const char* str: String to check for compatibility
 * - const help_t* info: Pointer to the help_t structure containing constraints
 * - size_t k: Length of the string
 * Returns: true if the string is compatible, false otherwise
 */
bool compatible(const char * str, const help_t * info, size_t k);

/*
 * Updates the filter of a radix trie node based on the current constraints.
 * Parameters:
 * - rax_t* root: Pointer to the root node of the radix trie
 * - int* str_occur: Array tracking character occurrences in the string
 * - int curr_idx: Current index in the string
 * - help_t* info: Pointer to the help_t structure containing constraints
 * - int game: Game index for filtering
 * Returns: size of the filtered dictionary after the update
 */
size_t update_filter(rax_t * root, int * str_occur, size_t curr_idx, help_t * info, size_t game); 

#endif 