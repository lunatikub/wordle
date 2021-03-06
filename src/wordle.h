#ifndef __WORDLE__
#define __WORDLE__

#include <stdbool.h>

#include "words.h"
#include "color.h"
#include "status.h"

struct wordle {
  const struct word *words;
  char candidate[MAX_WORD_LEN];
  enum status current[MAX_WORD_LEN];
  char right[MAX_WORD_LEN];
  bool wrong[ALPHA_SZ];
  bool wrong_location[ALPHA_SZ][MAX_WORD_LEN];
  bool discarded[ALPHA_SZ];
};

/**
 * Initialize and check if the list of words is valid.
*/
bool wordle_init(struct wordle *wordle, const struct word *words);

/**
 * Update status with the current candidate results.
 */
void wordle_update_status(struct wordle *wordle);

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
const char* wordle_find_next_candidate(struct wordle *wordle);

/**
 * Return the number of candidates.
 */
unsigned wordle_count_number_of_candidates(struct wordle *wordle);

/**
 * Set the next candidate.
 */
void wordle_set_candidate(struct wordle *wordle, const char *candidate);

/**
 * Dump status:
 *  + discarded characters.
 *  + wrong character locations.
 *  + right character locations.
 */
void wordle_dump_status(struct wordle *wordle, unsigned round);

#endif /* !__WORDLE__ */
