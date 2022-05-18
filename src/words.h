#ifndef __WORD__
#define __WORD__

#include <stdbool.h>

#define ALPHA_SZ 26
#define MAX_WORD_LEN 10

struct word {
  const char **list;
  unsigned nr;
  unsigned len;
};

/**
 * Check the list of words.
 * Word can only contains [a-z] characters.
 */
bool words_check_list(struct word *words);

/**
 * Find the best first candidate from a list of words.
 * Based on:
 *  + frequencies of the letters.
 *  + positions of the letters.
 *  + non-repetition of the letters.
 */
const char* words_find_best_candidate(struct word *words);

#endif /* !__WORD__ */
