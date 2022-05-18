#ifndef __WORD__
#define __WORD__

#include <stdbool.h>

#include "lang.h"

#define ALPHA_SZ 26
#define MAX_WORD_LEN 10

struct word {
  char *name;
  const char **list;
  unsigned nr;
  unsigned len;
  enum lang lang;
};

/**
 * Check the list of words.
 * Word can only contains [a-z] characters.
 */
bool words_check_list(const struct word *words);

/**
 * Find the best first candidate from a list of words.
 * Based on:
 *  + frequencies of the letters.
 *  + positions of the letters.
 *  + non-repetition of the letters.
 */
const char* words_find_best_candidate(const struct word *words);

/**
 * Find the list of words correspondong the parameters
 * given in argument. Return NULL if not found.
 */
const struct word* words_find(const char *name, enum lang lang, unsigned len);

#endif /* !__WORD__ */
