#include <stdlib.h>
#include <string.h>

#include "words.h"

#include "words/wordleplay_fr_4_words.h"
#include "words/wordleplay_fr_5_words.h"
#include "words/wordleplay_fr_6_words.h"
#include "words/wordleplay_en_4_words.h"
#include "words/wordleplay_en_5_words.h"
#include "words/wordleplay_en_6_words.h"
#include "words/wordle_louan_fr_5_words.h"

struct stats {
  unsigned freq[ALPHA_SZ]; /* letter frequencies */
  unsigned pos[ALPHA_SZ][MAX_WORD_LEN]; /* letter positions */
};

static void words_process_letter_stats(const struct word *words, struct stats *stats)
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

const char* words_find_best_candidate(const struct word *words)
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

bool words_check_list(const struct word *words)
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

const struct word* words_find(const char *name, enum lang lang, unsigned len)
{
  static const struct word *all_words[] = {
    &wordleplay_fr_4,
    &wordleplay_fr_5,
    &wordleplay_fr_6,
    &wordleplay_en_4,
    &wordleplay_en_5,
    &wordleplay_en_6,
    &wordle_louan_fr_5,
  };

  for (unsigned i = 0; i < sizeof(all_words) / sizeof(const struct word*); ++i) {
    const struct word *words = all_words[i];
    if (strcmp(words->name, name) == 0 &&
        lang == words->lang &&
        len == words->len) {
      return words;
    }
  }
  return NULL;
}
