#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h> 

#include "rax.h"
#include "memory_pool.h"


rax_t* rax_alloc_node()
{
    rax_t * new_node = (rax_t *) malloc(sizeof(rax_t)); 

    // If the allocation is successful, initialize the node's fields
    if (new_node != NULL) {
        new_node->filter = 0; 
        new_node->sibling = NULL; 
        new_node->child = NULL; 
        new_node->base = 0;
        new_node->size = 0;
    } else {
        fprintf(stderr, "failed allocation of new_node in rax_alloc_node\n"); 
    }

    return new_node; 
}


void rax_dealloc_node(rax_t* node_del)
{   
    // If the node is not NULL, free the memory allocated for it
    if (node_del == NULL) return; 
    free(node_del); 
}


void rax_dealloc(rax_t* root)
{
    if (root->sibling != NULL) rax_dealloc(root->sibling); 
    if (root->child != NULL) rax_dealloc(root->child); 

    rax_dealloc_node(root); 
}


bool rax_search(const rax_t* root, const char* my_str, size_t curr_idx, const memory_pool_t* pool)
{
    size_t piece_idx, new_idx;
    rax_t * good_child;  

    // If the string does not partially match the piece of the current node, return false
    for(piece_idx = 0; piece_idx != root->size; piece_idx++) {
        if (memory_pool_access(pool, root->base, piece_idx) != my_str[curr_idx + piece_idx]) {
            return false; 
        }
    }

    // If the string is fully matched, return true
    new_idx = curr_idx + piece_idx; 
    if (my_str[new_idx] == '\0') return true; 

    // The string is only partially matched, so we need to search for the next node to continue with in the linked list of children.
    good_child = rax_search_child(root->child, my_str[new_idx], NULL, pool); 
    if (good_child == NULL) return false; 
    return rax_search(good_child, my_str, new_idx, pool); 
}


void rax_insert(rax_t * root, const char* my_str, size_t curr_idx, size_t str_size, size_t game, memory_pool_t* pool)
{
    size_t piece_idx, new_idx, old_sign;  
    rax_t * new_node, * new_node_son, * old_child, * child_find, * prev, * old_sibling, * new_root;  

    old_sign = root->filter; 
    if (game == 0) {
        root->filter = 0; 
    }

    size_t piece_size = root->size; 

    for(piece_idx = 0; piece_idx != piece_size; piece_idx++) {
        if (memory_pool_access(pool, root->base, piece_idx) != my_str[curr_idx + piece_idx]) {
            // allocating the new nodes
            new_node = rax_alloc_node(); 
            new_node_son = rax_alloc_node();

            // copying the correct substrings into the nodes
            // substring_copy(new_node->piece, my_str, piece_idx + curr_idx, str_size); 
            // substring_copy(new_node_son->piece, root->piece, piece_idx, piece_size);

            memory_pool_request(pool, my_str + piece_idx + curr_idx, str_size - piece_idx - curr_idx, &new_node->base);
            new_node->size = str_size - piece_idx - curr_idx;
            new_node_son->base = root->base + piece_idx;
            new_node_son->size = piece_size - piece_idx;

            root->size = piece_idx;

            // saving the children of root 
            old_child = root->child; 
            root->child = NULL; 

            // set the filters
            new_node->filter = game;
            new_node_son->filter = old_sign;
            if (game == 0) root->filter = 0;   

            // setting the children of root
            root->child = rax_insert_child(root->child, new_node, pool); 
            root->child = rax_insert_child(root->child, new_node_son, pool); 
            new_node_son->child = old_child; 

            return; 
        }
    }

    new_idx = curr_idx + piece_size; 

    // se si entra qui vuol dire che la stringa da inserire è già presente
    if (new_idx == str_size) return; 

    child_find = rax_search_child(root->child, my_str[new_idx], &prev, pool); 

    if (child_find == NULL) {
        // create the new node and fill it with its substring
        new_node = rax_alloc_node(); 
        //printf("entering here %zu\n", new_idx); 
        //printf("inserting string %s\n", my_str + new_idx);

        // substring_copy(new_node->piece, my_str, new_idx, str_size); 
        memory_pool_request(pool, my_str + new_idx, str_size - new_idx, &new_node->base);
        new_node->size = str_size - new_idx;
        new_node->filter = game; 

        // insert the new node as a child of root
        root->child = rax_insert_child(root->child, new_node, pool); 
        return; 
    }

    rax_insert(child_find, my_str, new_idx, str_size, game, pool); 
}


void rax_print(rax_t * root, char my_str[], int curr_idx, int game, memory_pool_t* pool)
{
    if (root->filter == game) return; 

    int piece_idx, new_idx;
    rax_t * tmp;   
    for(piece_idx = 0; piece_idx != root->size; piece_idx++) {
        my_str[curr_idx + piece_idx] = memory_pool_access(pool, root->base, piece_idx); 
    }

    new_idx = piece_idx + curr_idx; 
    tmp = root->child;

    if (tmp == NULL) {
        my_str[new_idx] = '\0'; 
        printf("%s\n", my_str);
        return;  
    }

    while(tmp != NULL) {
        rax_print(tmp, my_str, new_idx, game, pool); 
        tmp = tmp->sibling; 
    }
}


int rax_size(rax_t * root, int game)
{
    if (root->filter == game) return 0; 

    int ans = 0; 
    rax_t * tmp; 

    tmp = root->child; 
    if (tmp == NULL) return 1; 
    while(tmp != NULL) {
        ans += rax_size(tmp, game); 
        tmp = tmp->sibling; 
    }

    return ans; 
}


rax_t* rax_search_child(rax_t* child, char to_find, rax_t** prev, const memory_pool_t* pool)
{
    if (prev != NULL) *prev = NULL; 
    while(child != NULL) {
        if (memory_pool_access(pool, child->base, 0) == to_find) return child; 
        if (prev != NULL) *prev = child; 
        child = child->sibling; 
    } 

    return NULL; 
}


rax_t * rax_insert_child(rax_t * child, rax_t * to_ins, const memory_pool_t* pool)
{
    if (child == NULL) return to_ins; 
    char new_val = memory_pool_access(pool, to_ins->base, 0);
    rax_t * tmp = child;  

    if (new_val > memory_pool_access(pool, tmp->base, 0)) {
        while(tmp->sibling != NULL && new_val > memory_pool_access(pool, tmp->sibling->base, 0)) {
            tmp = tmp->sibling; 
        }

        to_ins->sibling = tmp->sibling;
        tmp->sibling = to_ins; 
        return child; 
    } else {
        to_ins->sibling = child; 
        return to_ins; 
    }
}