#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"
#include "constants.h"

static size_t char_index(char c);
static void reset(char *, int *, size_t); 

static size_t char_index(char c) {
  int char_index;

  if (c == '-') {
    char_index = 0;
  } else if (c - '0' >= 0 && c - '0' < 10) {
    char_index = c - '0' + 1;
  } else if (c - 'A' >= 0 && c - 'A' < 26) {
    char_index = c - 'A' + 11;
  } else if (c == '_') {
    char_index = 37;
  } else if (c - 'a' >= 0 && c - 'a' < 26) {
    char_index = c - 'a' + 38;
  } else {
    char_index = ALPHABET_SIZE;
    fprintf(stderr, "error: failed conversion of %c\n", c);
  }

  return char_index;
}

static void reset(char * str, int * occur, size_t up) {
  for (int i = 0; i <= up; i++) {
    if (str[i] != '\0')
      occur[char_index(str[i])]--;
  }
}

void substring_copy(char *dest, const char *source, size_t a, size_t b) {
  for (int i = 0; i < b - a; i++) {
    dest[i] = source[a + i];
  }
  dest[b - a] = '\0';
}

help_t *help_alloc(size_t k) {
  help_t *info = (help_t *)malloc(sizeof(help_t));
  info->forced = (char *)malloc((k + 1) * sizeof(char));
  info->appear = (bool *)malloc(k * ALPHABET_SIZE * sizeof(bool));

  return info;
}

void help_reset(help_t *info, size_t k) {
  for (size_t i = 0; i < ALPHABET_SIZE; i++) {
    info->option[i] = false;
    info->occur[i] = 0;
  }

  for (size_t i = 0; i < k; i++) {
    info->forced[i] = FOO;
    for (int j = 0; j < ALPHABET_SIZE; j++) {
      info->appear[i * ALPHABET_SIZE + j] = true;
    }
  }
  info->forced[k] = '\0';
}

void help_dealloc(help_t *info) {
  if (info == NULL)
    return;
  if (info->forced != NULL)
    free(info->forced);
  if (info->appear != NULL)
    free(info->appear);

  free(info);
}

void gen_constraint(const char * ref, const char * guess, char * constraint, help_t *info, size_t k) {
  int ref_occur[ALPHABET_SIZE] = {0}, guess_occur_notslash[ALPHABET_SIZE] = {0};

  for (size_t i = 0; i < k; i++) {
    constraint[i] = FOO;
    ref_occur[char_index(ref[i])]++;

    if (ref[i] == guess[i]) {
      constraint[i] = '+';
      info->forced[i] = ref[i];
      ref_occur[char_index(ref[i])]--;
      guess_occur_notslash[char_index(ref[i])]++;
    } else {
      info->appear[i * ALPHABET_SIZE + char_index(guess[i])] = false;
    }
  }

  for (size_t i = 0; i < k; i++) {
    if (constraint[i] != '+') {
      if (ref_occur[char_index(guess[i])] != 0) {
        constraint[i] = '|';
        guess_occur_notslash[char_index(guess[i])]++;
        ref_occur[char_index(guess[i])]--;

        if (!info->option[char_index(guess[i])]) {
          if (guess_occur_notslash[char_index(guess[i])] >
              info->occur[char_index(guess[i])]) {
            info->occur[char_index(guess[i])] =
                guess_occur_notslash[char_index(guess[i])];
          }
        }
      } else {
        constraint[i] = '/';
        info->option[char_index(guess[i])] = true;
        info->occur[char_index(guess[i])] =
            guess_occur_notslash[char_index(guess[i])];
      }
    }
  }

  constraint[k] = '\0';
}

bool compatible(const char *str, const help_t *info, size_t k) {
  size_t str_occur[ALPHABET_SIZE] = {0};

  for (size_t i = 0; i < k; i++) {
    str_occur[char_index(str[i])]++;
    if (info->forced[i] != FOO) {
      if (info->forced[i] != str[i])
        return false;
    } else {
      if (!info->appear[i * ALPHABET_SIZE + char_index(str[i])])
        return false;
    }
  }

  for (size_t i = 0; i < ALPHABET_SIZE; i++) {
    if (info->option[i]) {
      if (info->occur[i] != str_occur[i])
        return false;
    } else {
      if (info->occur[i] > str_occur[i])
        return false;
    }
  }

  return true;
}

size_t better_update_filter(rax_t *root, int *str_occur, int curr_idx, help_t *info, int game) {
  if (root->filter == game)
    return 0;

  size_t piece_idx, ans = 0;

  rax_t *tmp;
  size_t piece_size = strlen(root->piece);

  for (piece_idx = 0; root->piece[piece_idx] != '\0'; piece_idx++) {

    str_occur[char_index(root->piece[piece_idx])]++;

    if (info->forced[curr_idx + piece_idx] != FOO) {
      if (info->forced[curr_idx + piece_idx] != root->piece[piece_idx]) {
        root->filter = game;
        reset(root->piece, str_occur, piece_idx);
        return 0;
      }
    } else {
      if (!info->appear[(curr_idx + piece_idx) * ALPHABET_SIZE +
                        char_index(root->piece[piece_idx])]) {
        root->filter = game;
        reset(root->piece, str_occur, piece_idx);
        return 0;
      }
    }

    for (size_t i = 0; i < ALPHABET_SIZE; i++) {
      if (info->option[i] && str_occur[i] > info->occur[i]) {
        root->filter = game;
        reset(root->piece, str_occur, piece_idx);
        return 0;
      }
    }
  }

  tmp = root->child;
  if (tmp == NULL) {
    for (size_t i = 0; i < ALPHABET_SIZE; i++) {
      if (info->option[i]) {
        if (str_occur[i] != info->occur[i]) {
          root->filter = game;
          reset(root->piece, str_occur, piece_idx);
          return 0;
        }
      } else {
        if (str_occur[i] < info->occur[i]) {
          root->filter = game;
          reset(root->piece, str_occur, piece_idx);
          return 0;
        }
      }
    }

    root->filter = 0;
    reset(root->piece, str_occur, piece_idx);
    return 1;
  } else {
    while (tmp != NULL) {
      ans += better_update_filter(tmp, str_occur, curr_idx + piece_idx, info,
                                  game);
      tmp = tmp->sibling;
    }

    if (ans == 0)
      root->filter = game;

    reset(root->piece, str_occur, piece_idx);
    return ans;
  }
}
