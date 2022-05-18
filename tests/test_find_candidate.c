#include <stdlib.h>

#include "words/wordleplay_en_5_words.h"
#include "words/wordleplay_fr_5_words.h"

#include "words.h"
#include "test.h"

TEST_F(find_candidate, en_5)
{
  const struct word *words = &wordleplay_en_5;
  const char *candidate = words_find_best_candidate(words);
  return (strcmp(candidate, "tares") == 0);
}

TEST_F(find_candidate, fr_5)
{
  const struct word *words = &wordleplay_fr_5;
  const char *candidate = words_find_best_candidate(words);
  return (strcmp(candidate, "raies") == 0);
}

const static struct test find_candidate_tests[] = {
  TEST(find_candidate, en_5),
  TEST(find_candidate, fr_5),
};

TEST_SUITE(find_candidate);
