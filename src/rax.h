#ifndef RAX_H
#define RAX_H

#include <stdbool.h>
#include <stdlib.h>

/* Radix trie node structure.
 * Members:
 * - size_t filter: Filter value for the node
 * - rax_t* sibling: Pointer to the next sibling node
 * - rax_t* child: Pointer to the first child node
 * - char* piece: String piece stored in this node
 */
typedef struct rax_t {
    size_t filter;
    struct rax_t* child;  
    struct rax_t* sibling; 
    char piece[]; 
} rax_t; 


/* Allocates a new radix trie node. Ownership of rax_t node allocated is transferred to the caller.
 * Parameters:
 * - size_t k: Size of the string piece to store
 * Returns: Pointer to the newly allocated node, or NULL on failure
 */
rax_t* rax_alloc_node(size_t k); 


/* Deallocates a single radix trie node. Ownership of rax_t node deallocated is transferred to the callee.
 * Parameters:
 * - rax_t* node_del: Node to deallocate
 */
void rax_dealloc_node(rax_t* node_del); 


/* Recursively deallocates an entire radix trie. Ownership of the entire radix trie deallocated is transferred to the callee.
 * Parameters:
 * - rax_t* root: Root node of the trie to deallocate
 */
void rax_dealloc(rax_t* root); 


/* Searches for a string in the radix trie.
 * Parameters:
 * - rax_t* root: Root node of the trie
 * - char* my_str: String to search for
 * - size_t curr_idx: Current index in the string
 * Returns: true if string is found, false otherwise
 */
bool rax_search(const rax_t* root, const char* my_str, size_t curr_idx); 


void rax_insert(rax_t*, const char*, size_t, size_t, size_t); 
void rax_print(rax_t*, char*, int, int); 
int rax_size(rax_t*, int); 

rax_t* rax_search_child(rax_t*, char, rax_t**); 
rax_t* rax_insert_child(rax_t*, rax_t*); 

#endif 