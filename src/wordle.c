#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "wordle.h"

bool wordle_init(struct wordle *wordle, struct word *words)
{
  memset(wordle, 0, sizeof(*wordle));
  wordle->words = words;
  return words_check_list(wordle->words);
}

void wordle_update_status(struct wordle *wordle)
{
  for (unsigned i = 0; i < wordle->words->len; ++i) {
    unsigned c = wordle->candidate[i] - 'a';

    switch (wordle->current[i]) {
      case DISCARDED:
        wordle->discarded[c] = true;
        break;
      case RIGHT:
        wordle->wrong[c] = false;
        wordle->right[i] = wordle->candidate[i];
        break;
      case WRONG:
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

  for (i = 0; i < wordle->words->len; ++i) {
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
      for (unsigned j = 0; j < wordle->words->len; ++j) {
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

const char* wordle_find_next_candidate(struct wordle *wordle)
{
  for (unsigned i = 0; i < wordle->words->nr; ++i) {
    if (word_is_valid(wordle, wordle->words->list[i]) == true) {
      return wordle->words->list[i];
    }
  }
  return NULL;
}

unsigned wordle_count_number_of_candidates(struct wordle *wordle)
{
  unsigned count = 0;

  for (unsigned i = 0; i < wordle->words->nr; ++i) {
    if (word_is_valid(wordle, wordle->words->list[i]) == true) {
      ++count;
    }
  }
  return count;
}

void wordle_set_candidate(struct wordle *wordle, const char *candidate)
{
  printf("[wordle] set next candidate: %s", candidate);
  memcpy(wordle->candidate, candidate, wordle->words->len);
}

void wordle_dump_location_status(enum status status)
{
  switch (status) {
    case RIGHT:
      printf("\033[0;32mO");
      break;
    case WRONG:
      printf("\033[0;33mX");
      break;
    case DISCARDED:
      printf("_");
      break;
    default:
      assert(!"location name not found...");
  };
  printf("\033[0m");
}

enum status wordle_map_from_color(struct color *color,
                                  const struct color *c_right,
                                  const struct color *c_wrong,
                                  const struct color *c_discarded)
{
  if (color_approx_eq(color, c_right) == true) {
    return RIGHT;
  }
  if (color_approx_eq(color, c_wrong) == true) {
    return WRONG;
  }
  if (color_approx_eq(color, c_discarded) == true) {
    return DISCARDED;
  }
  return UNKNOWN;
}
