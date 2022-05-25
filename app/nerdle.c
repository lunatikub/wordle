#include <stdio.h>

#include "nerdle/nerdle.h"
#include "nerdle/equation.h"

int main(void)
{
  struct nerdle *nerdle = nerdle_create(5);

  nerdle->right[2] = '2';
  nerdle->right[3] = '=';
  nerdle->discarded[nerdle_map_alpha('1')] = true;
  nerdle->discarded[nerdle_map_alpha('+')] = true;
  nerdle->discarded[nerdle_map_alpha('3')] = true;
  /* nerdle->discarded[nerdle_map_alpha('-')] = true; */
  /* nerdle->wrong[nerdle_map_alpha('1')][0] = true; */
  /* nerdle->wrong[nerdle_map_alpha('2')][3] = true; */
  /* nerdle->wrong[nerdle_map_alpha('=')][4] = true; */
  /* nerdle->wrong[nerdle_map_alpha('8')][5] = true; */

  nerdle_generate_equations(nerdle);

  printf("nr_candidate: %u\n", nerdle->nr_candidate);
  struct candidate *candidate;
  STAILQ_FOREACH(candidate, &nerdle->candidates, next) {
    printf("%s\n", candidate->equation);
  }
  printf("best candidate: %s\n", nerdle_find_best_equation(nerdle));

  nerdle->discarded[nerdle_map_alpha('4')] = true;
  nerdle->discarded[nerdle_map_alpha('*')] = true;
  nerdle->discarded[nerdle_map_alpha('8')] = true;

  nerdle_eliminate_candidates(nerdle);

  printf("nr_candidate: %u\n", nerdle->nr_candidate);
  STAILQ_FOREACH(candidate, &nerdle->candidates, next) {
    if (candidate->eliminated == false) {
      printf("%s\n", candidate->equation);
    }
  }
  printf("best candidate: %s\n", nerdle_find_best_equation(nerdle));

  nerdle->right[1] = '-';
  nerdle->discarded[nerdle_map_alpha('5')] = true;
  nerdle->discarded[nerdle_map_alpha('7')] = true;

  nerdle_eliminate_candidates(nerdle);

  printf("nr_candidate: %u\n", nerdle->nr_candidate);
  STAILQ_FOREACH(candidate, &nerdle->candidates, next) {
    if (candidate->eliminated == false) {
      printf("%s\n", candidate->equation);
    }
  }
  printf("best candidate: %s\n", nerdle_find_best_equation(nerdle));

  nerdle_destroy(nerdle);

  return 0;
}
