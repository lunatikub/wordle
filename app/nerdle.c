#include <stdio.h>

#include "nerdle/nerdle.h"
#include "nerdle/equation.h"

int main(void)
{
  struct nerdle *nerdle = nerdle_create(6);

  nerdle->right[5] = '8';
  nerdle->discarded[nerdle_map_alpha('0')] = true;
  nerdle->discarded[nerdle_map_alpha('1')] = true;
  nerdle->discarded[nerdle_map_alpha('-')] = true;

  nerdle_generate_equations(nerdle);

  printf("nr_candidate: %u\n", nerdle->nr_candidate);
  struct candidate *candidate;
  STAILQ_FOREACH(candidate, &nerdle->candidates, next) {
    printf("%s\n", candidate->equation);
  }

  printf("best candidate: %s\n", nerdle_find_best_equation(nerdle));

  nerdle_destroy(nerdle);

  return 0;
}
