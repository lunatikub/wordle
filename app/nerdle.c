#include <stdio.h>

#include "nerdle/nerdle.h"
#include "nerdle/equation.h"

static void debug(struct nerdle *nerdle)
{
  printf("nr_candidate: %u\n", nerdle->nr_candidate);
  struct candidate *candidate = nerdle->candidates;
  while (candidate != NULL) {
    printf("%s\n", candidate->equation);
    candidate = candidate->next;
  }
  printf("best candidate: %s\n", nerdle_find_best_equation(nerdle));
}

/* nerdle->right[2] = '2'; */
/* nerdle->discarded[nerdle_map_alpha('3')] = true; */
/* nerdle->wrong[nerdle_map_alpha('8')][5] = true; */

int main(void)
{
  struct nerdle *nerdle = nerdle_create(5);

  nerdle->right[2] = '2';
  nerdle->right[3] = '=';
  nerdle->discarded[nerdle_map_alpha('1')] = true;
  nerdle->discarded[nerdle_map_alpha('+')] = true;
  nerdle->discarded[nerdle_map_alpha('3')] = true;

  nerdle_generate_equations(nerdle);
  debug(nerdle);

  nerdle->discarded[nerdle_map_alpha('4')] = true;
  nerdle->discarded[nerdle_map_alpha('*')] = true;
  nerdle->discarded[nerdle_map_alpha('8')] = true;

  nerdle_select_equations(nerdle);
  debug(nerdle);

  nerdle->right[1] = '-';
  nerdle->discarded[nerdle_map_alpha('5')] = true;
  nerdle->discarded[nerdle_map_alpha('7')] = true;

  nerdle_select_equations(nerdle);
  debug(nerdle);

  nerdle_destroy(nerdle);

  return 0;
}
