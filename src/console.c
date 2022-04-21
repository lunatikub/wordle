#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "wordle.h"

#include "fr_words_5.h"

static void scan_inputs(struct wordle *wordle, unsigned round)
{
  int n;
  printf("\n[%u] word ? ", round);
  n = scanf("%s", wordle->candidate);
  assert(n == 1);
  printf("[%u] status ? ", round);
  n = scanf("%u %u %u %u %u",
            &wordle->current[0], &wordle->current[1],
            &wordle->current[2], &wordle->current[3],
            &wordle->current[4]);
  assert(n == WORD_LEN);
  printf("[%u] tried: %s -> %u %u %u %u %u\n",
         round, wordle->candidate,
         wordle->current[0], wordle->current[1],
         wordle->current[2], wordle->current[3],
         wordle->current[4]);
}

int main(void)
{
  struct wordle wordle;
  if (wordle_init(&wordle, words, NR_WORD) == false) {
    return -1;
  }

  printf("<<< wordle >>>\n");
  printf("+ discarded:      %i\n", DISCARDED);
  printf("+ wrong location: %i\n", WRONG_LOCATION);
  printf("+ right location: %i\n", RIGHT_LOCATION);
  printf("suggested first word: tarie\n");

  for (unsigned round = 1; round <= NR_MAX_ROUND; ++round) {
    scan_inputs(&wordle, round);
    update_status(&wordle);
    dump_status(&wordle, round, "console");
    dump_next_candidates(&wordle);
  }

  return 0;
}
