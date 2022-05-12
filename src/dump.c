#include <stdio.h>

#include "wordle.h"

static void dump_discarded(struct wordle *wordle, unsigned round)
{
  printf("[wordle] {round:%u} discarded: { ", round);
  for (unsigned i = 0; i < ALPHA_SZ; ++i) {
    if (wordle->discarded[i] == true) {
      printf("%c,", i + 'a');
    }
  }
  printf(" }\n");
}

static void dump_wrong_location(struct wordle *wordle, unsigned round)
{
  printf("[wordle] {round:%u} wrong location: { ", round);
  for (unsigned i = 0; i < ALPHA_SZ; ++i) {
    if (wordle->wrong[i] == true) {
      printf("%c:[", i + 'a');
      for (unsigned j = 0; j < wordle->words->len; ++j) {
        printf("%c,", wordle->wrong_location[i][j] == true  ? 'X' : 'O');
      }
      printf("], ");
    }
  }
  printf(" }\n");
}

static void dump_right_location(struct wordle *wordle, unsigned round)
{
  printf("[wordle] {round:%u} right location: { ", round);
  for (unsigned i = 0; i < wordle->words->len; ++i) {
    printf("%c,", wordle->right[i] == 0  ? '_' : wordle->right[i]);
  }
  printf(" }\n");
}

void wordle_dump_status(struct wordle *wordle, unsigned round)
{
  dump_discarded(wordle, round);
  dump_wrong_location(wordle, round);
  dump_right_location(wordle, round);
}
