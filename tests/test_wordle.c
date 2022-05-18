#include <wordle.h>
#include "words/wordleplay_fr_6_words.h"

#include "test.h"

#define D DISCARDED
#define W WRONG
#define R RIGHT

static void set_status(struct wordle *wordle, enum status *status, unsigned nr)
{
  for (unsigned i = 0; i < nr; ++i) {
    wordle->current[i] = status[i];
  }
}

struct round {
  char *candidate;
  enum status *status;
};

/**
 * taries DDWWWD
 * baisai DDWDDD
 * maison DDWDDD
 * saison DDWDDD
 * membre DWDDRR
 * pierre is not anymore a candidate !
 */
TEST_F(wordle, pierre)
{
  const struct word *words = &wordleplay_fr_6;
  struct wordle wordle;
  if (wordle_init(&wordle, words) == false) {
    return false;
  }

  static enum status status_r1[] = { D, D, W, W, W, D };
  static enum status status_r2[] = { D, D, W, D, D, D };
  static enum status status_r3[] = { D, D, W, D, D, D };
  static enum status status_r4[] = { D, D, W, D, D, D };
  static enum status status_r5[] = { D, W, D, D, R, R };
  static enum status status_r6[] = { W, D, D, R, D, R };

  static struct round rounds[] = {
    { "taries", status_r1 },
    { "baisai", status_r2 },
    { "maison", status_r3 },
    { "saison", status_r4 },
    { "membre", status_r5 },
    { "entree", status_r6 },
  };

  for (unsigned r = 0; r < sizeof(rounds) / sizeof(struct round); ++r) {
    wordle_set_candidate(&wordle, rounds[r].candidate);
    set_status(&wordle, rounds[r].status, words->len);
    wordle_update_status(&wordle);
    wordle_dump_status(&wordle, r + 1);
  }

  return word_is_valid(&wordle, "pierre");
}

const static struct test wordle_tests[] = {
  TEST(wordle, pierre),
};

TEST_SUITE(wordle);
