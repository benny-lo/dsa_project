#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "rax.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  // initialize empty dictionary
  rax_t *dict = rax_alloc_node(0);

  // read the length of the strings
  size_t k;
  if (scanf("%zu", &k) != 1)
    fprintf(stderr, "error taking k\n");

  // initialize the input buffer
  size_t input_size = MAX_KEYWORD;
  if (k > input_size)
    input_size = k;
  char input[input_size + 1];

  // building the radix trie
  int dict_size = 0,
      game =
          0; // we can put the dict size in a management struct for radix trie
  if (scanf("%s", input) != 1)
    fprintf(stderr, "error taking input while building dict\n");
  while (input[0] != '+') {
    // insert the word into the dictionary and increment the dictionary size
    rax_insert(dict, input, 0, k, game);
    dict_size++;

    if (scanf("%s", input) != 1)
      fprintf(stderr, "error taking input while building dict\n");
  }

  // data structures to keep track of the constraints
  help_t *info = help_alloc(k);
  int str_occur[ALPHABET_SIZE] = {0};
  char constraint[k + 1], ref[k + 1], my_str[k + 1];
  int guess_counter = 0;
  int filtered_size, n;

  do {
    if (strncmp(input, NEW_GAME, strlen(NEW_GAME) + 1) == 0) {
      // new_game command

      // reset the filtered_size and count compare and change the game index
      filtered_size = dict_size;
      game++;
      guess_counter = 0;

      // reset the constraint info struct
      help_reset(info, k);

      if (scanf("%s", ref) != 1)
        fprintf(stderr, "error taking ref at beginning of new game\n");
      if (scanf("%d", &n) != 1)
        fprintf(stderr, "error taking n at beginning of new game\n");

    } else if (strncmp(input, INSERT_START, strlen(INSERT_START) + 1) == 0) {
      // insert_start command

      if (scanf("%s", input) == 0)
        fprintf(stderr, "error taking input during insertion\n");
      while (strncmp(input, INSERT_END, strlen(INSERT_END) + 1) != 0) {
        if (compatible(input, info, k)) {
          // if the input is compatible with the constraints, it will be part of
          // the filtered dictionary
          rax_insert(dict, input, 0, k, 0);
          filtered_size++;
        } else {
          // if the input is not compatible with the constraints, it will not be
          // part of the filtered dictionary
          rax_insert(dict, input, 0, k, game);
        }

        // increment the dictionary size because an element has been inserted in
        // the dictionary
        dict_size++;

        if (scanf("%s", input) != 1)
          fprintf(stderr, "error taking input during insertion\n");
      }

    } else if (strncmp(input, PRINT_FILTERED, strlen(PRINT_FILTERED) + 1) ==
               0) {
      // print_filtered command

      // print the strings in the dictionary that are part of the filtered
      // dictionary
      rax_print(dict, my_str, 0, game);

    } else {
      // processing a guess against the reference word

      // if the guess is the reference word, print ok and the game ends
      if (strncmp(input, ref, k + 1) == 0) {
        printf("ok\n");
        continue;
      }

      // if the guess is not present in the dictionary, print that it does not
      // exist
      if (rax_search(dict, input, 0) == 0) {
        printf("not_exists\n");
        continue;
      }

      // increment the count compare because a valid guess has been made
      guess_counter++;

      // generate the constraint and update the constraints
      gen_constraint(ref, input, constraint, info, k);
      printf("%s\n", constraint);

      // update the filtered dictionary and print its size
      filtered_size = better_update_filter(dict, str_occur, 0, info, game);
      printf("%d\n", filtered_size);

      // if the maximum number of guesses has been reached, end the game for ko
      if (guess_counter == n) {
        printf("ko\n");
        continue;
      }
    }

  } while (scanf("%s", input) != EOF);

  // deallocate the radix trie and the info struct
  rax_dealloc(dict);
  help_dealloc(info);

  return 0;
}
