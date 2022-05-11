#include <stdlib.h>

#include <wordle.h>
#include "fr_words_5.h"

#include "test.h"

static void set_discarded(struct wordle *wordle, char *discarded, unsigned nr_discarded)
{
  for (unsigned i = 0; i < nr_discarded; ++i) {
    wordle->discarded[discarded[i] - 'a'] = true;
  }
}

static void set_right_location(struct wordle *wordle, char *right)
{
  for (unsigned i = 0; i < wordle->len; ++i) {
    wordle->right[i] = right[i];
  }
}

static void set_wrong_location(struct wordle *wordle, bool *wrong,
                               bool wrong_location[ALPHA_SZ][5])
{
  for (unsigned i = 0; i < ALPHA_SZ; ++i) {
    wordle->wrong[i] = wrong[i];
    memcpy(wordle->wrong_location[i], wrong_location[i], wordle->len);
  }
}

TEST_F(word_is_valid, conte)
{
  struct wordle wordle;
  if (wordle_init(&wordle, words_5, 5, nr_word_5) == false) {
    printf("TOTO\n");
    return false;
  }
  char discarded[5] = { 'a', 'e', 'i', 'l', 'r' };
  set_discarded(&wordle, discarded, 5);
  char right[5] = { 0, 0, 'n', 't', 'e' };
  set_right_location(&wordle, right);
  return word_is_valid(&wordle, "conte");
}

TEST_F(word_is_valid, sueur)
{
  struct wordle wordle;
  if (wordle_init(&wordle, words_5, 5, nr_word_5) == false) {
    return false;
  }
  char discarded[] = { 'a', 'b', 'e', 'i', 'l', 't'};
  set_discarded(&wordle, discarded, sizeof(discarded));
  char right[5] = { 0, 0, 0, 0, 'r' };
  set_right_location(&wordle, right);
  bool wrong[ALPHA_SZ] = { false };
  wrong['e' - 'a'] = true;
  bool wrong_location[ALPHA_SZ][5];
  wrong_location['e' - 'a'][0] = false;
  wrong_location['e' - 'a'][1] = true;
  wrong_location['e' - 'a'][2] = false;
  wrong_location['e' - 'a'][3] = false;
  wrong_location['e' - 'a'][4] = true;
  set_wrong_location(&wordle, wrong, wrong_location);
  return word_is_valid(&wordle, "sueur");
}

const static struct test word_is_valid_tests[] = {
  TEST(word_is_valid, conte),
  TEST(word_is_valid, sueur),
};

TEST_SUITE(word_is_valid);
