#ifndef __WORDLE__
#define __WORDLE__

#include <stdbool.h>

#define ALPHA_SZ 26
#define NR_MAX_ROUND 6
#define WORD_LEN 5

enum status {
  UNKNOWN,
  DISCARDED,
  WRONG_LOCATION,
  RIGHT_LOCATION,
};

struct wordle {
  unsigned nr_word;
  const char **words;
  char candidate[WORD_LEN];
  enum status current[WORD_LEN];
  char right[WORD_LEN];
  bool wrong[ALPHA_SZ];
  bool wrong_location[ALPHA_SZ][WORD_LEN];
  bool discarded[ALPHA_SZ];
};

/**
 * Dump status:
 *  + discarded characters.
 *  + wrong character locations.
 *  + right character locations.
 */
void dump_status(struct wordle *wordle, unsigned round, const char *prefix);

/**
 * Update status with the current candidate results.
*/
void update_status(struct wordle *wordle);

/**
 * Initialize and chck if the list of words is valid:
 * only contains [a-z] characters.
*/
bool wordle_init(struct wordle *wordle, const char **words, unsigned nr_word);

/**
 * Invalid the word if:
 *  + its contains a discarded character.
 *  + character is at a wrong location.
 *  + its doesn't contain all mandatory characters.
 */
bool word_is_valid(struct wordle *wordle, const char *word);

/**
 * Find the next candidate.
 */
const char* find_next_candidate(struct wordle *wordle);

#endif /* !__WORDLE__ */
