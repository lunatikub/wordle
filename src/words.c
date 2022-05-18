#include <stdlib.h>
#include <string.h>

#include "words.h"

struct stats {
  unsigned freq[ALPHA_SZ]; /* letter frequencies */
  unsigned pos[ALPHA_SZ][MAX_WORD_LEN]; /* letter positions */
};

static void words_process_letter_stats(struct word *words, struct stats *stats)
{
  for (unsigned i = 0; i < words->nr; ++i) {
    const char *word = words->list[i];
    for (unsigned j = 0; j < words->len; ++j) {
      unsigned l = word[j] - 'a';
      ++stats->freq[l];
      ++stats->pos[l][j];
    }
  }
}

const char* words_find_best_candidate(struct word *words)
{
  struct stats letter_stats;
  memset(&letter_stats, 0, sizeof(struct stats));

  words_process_letter_stats(words, &letter_stats);

  unsigned best_candidate = 0;
  unsigned best_score = 0;

  for (unsigned i = 0; i < words->nr; ++i) {

    const char *word = words->list[i];
    unsigned score = 0;
    bool letters[ALPHA_SZ] = { false };

    for (unsigned j = 0; j < words->len; ++j) {
      unsigned l = word[j] - 'a';
      score += letter_stats.freq[l];
      score += letter_stats.pos[l][j];
      /* we want the maximum of different letters */
      if (letters[l] == true) {
        score = 0;
        break;
      }
      letters[l] = true;
    }

    if (score > best_score) {
      best_score = score;
      best_candidate = i;
    }
  }

  return words->list[best_candidate];
}

bool words_check_list(struct word *words)
{
  for (unsigned i = 0; i < words->nr; ++i) {
    for (unsigned j = 0; j < words->len; ++j) {
      char c = words->list[i][j];
      if (c < 'a' || c > 'z') {
        return false;
      }
    }
  }
  return true;
}
