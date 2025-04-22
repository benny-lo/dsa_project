#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h> 

#include "rax.h"
#include "utils.h"


rax_t* rax_alloc_node(size_t k)
{
    rax_t * new_node = (rax_t *) malloc(sizeof(rax_t) + k+1); 

    // If the allocation is successful, initialize the node's fields
    if (new_node != NULL) {
        new_node->sibling = NULL; 
        new_node->child = NULL; 
        new_node->piece[k] = '\0'; 
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


bool rax_search(const rax_t* root, const char* my_str, size_t curr_idx)
{
    size_t piece_idx, new_idx;
    rax_t * good_child;  

    // If the string does not partially match the piece of the current node, return false
    for(piece_idx = 0; root->piece[piece_idx] != '\0'; piece_idx++) {
        if (root->piece[piece_idx] != my_str[curr_idx + piece_idx]) return false; 
    }

    // If the string is fully matched, return true
    new_idx = curr_idx + piece_idx; 
    if (my_str[new_idx] == '\0') return true; 

    // The string is only partially matched, so we need to search for the next node to continue with in the linked list of children.
    good_child = rax_search_child(root->child, my_str[new_idx], NULL); 
    if (good_child == NULL) return false; 
    return rax_search(good_child, my_str, new_idx); 
}


void rax_insert(rax_t * root, const char* my_str, size_t curr_idx, size_t str_size, size_t game, hash_set_t * hash_set)
{
    size_t piece_idx, new_idx, old_sign;  
    rax_t * new_node, * new_node_son, * old_child, * child_find, * prev, * old_sibling, * new_root;  


    bool in_hash_set = hash_set_get(hash_set, (char *) root, &old_sign); 
    if (!in_hash_set) {
        old_sign = 0; 
    }
    
    if (game == 0) {
        hash_set_remove(hash_set, (char *) root); 
    }

    size_t piece_size = strlen(root->piece); 

    for(piece_idx = 0; root->piece[piece_idx] != '\0'; piece_idx++) {
        if (root->piece[piece_idx] != my_str[curr_idx + piece_idx]) {
            // allocating the new nodes
            new_node = rax_alloc_node(str_size - piece_idx - curr_idx); 
            new_node_son = rax_alloc_node(piece_size - piece_idx);

            // copying the correct substrings into the nodes
            substring_copy(new_node->piece, my_str, piece_idx + curr_idx, str_size); 
            substring_copy(new_node_son->piece, root->piece, piece_idx, piece_size);
            root->piece[piece_idx] = '\0'; 

            // saving the children of root 
            old_child = root->child; 
            root->child = NULL; 

            // set the filters
            if (game != 0) {
                hash_set_insert(hash_set, (char *) new_node, game); 
            }
            if (old_sign == game) {
                hash_set_insert(hash_set, (char *) new_node_son, old_sign); 
            }

            // setting the children of root
            root->child = rax_insert_child(root->child, new_node); 
            root->child = rax_insert_child(root->child, new_node_son); 
            new_node_son->child = old_child; 

            return; 
        }
    }

    new_idx = curr_idx + piece_size; 

    // se si entra qui vuol dire che la stringa da inserire è già presente
    if (new_idx == str_size) return; 

    child_find = rax_search_child(root->child, my_str[new_idx], &prev); 

    if (child_find == NULL) {
        // create the new node and fill it with its substring
        new_node = rax_alloc_node(str_size - new_idx); 
        substring_copy(new_node->piece, my_str, new_idx, str_size); 
        
        hash_set_insert(hash_set, (char *) new_node, game); 

        // insert the new node as a child of root
        root->child = rax_insert_child(root->child, new_node); 
        return; 
    }

    rax_insert(child_find, my_str, new_idx, str_size, game, hash_set); 
}


void rax_print(rax_t * root, char my_str[], int curr_idx, int game, hash_set_t * hash_set)
{
    size_t tmp_game;
    bool in_hash_set = hash_set_get(hash_set, (char *) root, &tmp_game); 
    if (!in_hash_set) {
        tmp_game = 0;
    }

    if (tmp_game == game) return; 

    int piece_idx, new_idx;
    rax_t * tmp;   
    for(piece_idx = 0; root->piece[piece_idx] != '\0'; piece_idx++) {
        my_str[curr_idx + piece_idx] = root->piece[piece_idx]; 
    }

    new_idx = piece_idx + curr_idx; 
    tmp = root->child;

    if (tmp == NULL) {
        my_str[new_idx] = '\0'; 
        printf("%s\n", my_str);
        return;  
    }

    while(tmp != NULL) {
        rax_print(tmp, my_str, new_idx, game, hash_set); 
        tmp = tmp->sibling; 
    }
}


int rax_size(rax_t * root, int game, hash_set_t * hash_set)
{
    size_t tmp_game;
    bool in_hash_set = hash_set_get(hash_set, (char *) root, &tmp_game); 
    if (!in_hash_set) {
        tmp_game = 0;
    }

    if (tmp_game == game) return 0; 

    int ans = 0; 
    rax_t * tmp; 

    tmp = root->child; 
    if (tmp == NULL) return 1; 
    while(tmp != NULL) {
        ans += rax_size(tmp, game, hash_set); 
        tmp = tmp->sibling; 
    }

    return ans; 
}


rax_t* rax_search_child(rax_t* child, char to_find, rax_t** prev)
{
    if (prev != NULL) *prev = NULL; 
    while(child != NULL) {
        if (child->piece[0] == to_find) return child; 
        if (prev != NULL) *prev = child; 
        child = child->sibling; 
    } 

    return NULL; 
}


rax_t * rax_insert_child(rax_t * child, rax_t * to_ins)
{
    if (child == NULL) return to_ins; 
    char new_val = to_ins->piece[0];
    rax_t * tmp = child;  

    if (new_val > child->piece[0]) {
        while(tmp->sibling != NULL && new_val > tmp->sibling->piece[0]) {
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