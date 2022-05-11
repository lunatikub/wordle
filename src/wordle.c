#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "wordle.h"

static bool check_word_list(struct wordle *wordle)
{
  for (unsigned i = 0; i < wordle->nr_word; ++i) {
    for (unsigned j = 0; j < wordle->len; ++j) {
      char c = wordle->words[i][j];
      if (c < 'a' || c > 'z') {
        return false;
      }
    }
  }
  return true;
}

bool wordle_init(struct wordle *wordle, const char **words, unsigned len, unsigned nr_word)
{
  memset(wordle, 0, sizeof(*wordle));
  wordle->nr_word = nr_word;
  wordle->words = words;
  wordle->len = len;
  return check_word_list(wordle);
}

void update_status(struct wordle *wordle)
{
  for (unsigned i = 0; i < wordle->len; ++i) {
    unsigned c = wordle->candidate[i] - 'a';

    switch (wordle->current[i]) {
      case DISCARDED:
        wordle->discarded[c] = true;
        break;
      case RIGHT_LOCATION:
        wordle->wrong[c] = false;
        wordle->right[i] = wordle->candidate[i];
        break;
      case WRONG_LOCATION:
        wordle->wrong[c] = true;
        wordle->wrong_location[c][i] = true;
        break;
      default:
        assert(!"current[i] is not available...");
    };
  }
}

bool word_is_valid(struct wordle *wordle, const char *word)
{
  unsigned i = 0;

  for (i = 0; i < wordle->len; ++i) {
    unsigned c = word[i] - 'a';
    if (wordle->right[i] != 0) {
      if (wordle->right[i] != word[i]) {
        return false;
      }
    } else if (wordle->discarded[c] == true && wordle->wrong[c] == false) {
      return false;
    }
    if (wordle->wrong[c] == true && wordle->wrong_location[c][i] == true) {
      return false;
    }
  }

  for (i = 0; i < ALPHA_SZ; ++i) {
    if (wordle->wrong[i] == true) {
      bool in = false;
      for (unsigned j = 0; j < wordle->len; ++j) {
        unsigned c = word[j] - 'a';
        if (c == i) {
          in = true;
        }
      }
      if (in == false) {
        return false;
      }
    }
  }

  return true;
}

const char* find_next_candidate(struct wordle *wordle)
{
  for (unsigned i = 0; i < wordle->nr_word; ++i) {
    if (word_is_valid(wordle, wordle->words[i]) == true) {
      return wordle->words[i];
    }
  }
  return NULL;
}
