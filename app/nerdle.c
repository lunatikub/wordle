#include <stdio.h>

#include "nerdle/nerdle.h"
#include "nerdle/equation.h"

int main(void)
{
  struct nerdle *nerdle = nerdle_create(7);

  nerdle->right[2] = '-';
  nerdle->discarded[nerdle_map_alpha('1')] = true;
  nerdle->discarded[nerdle_map_alpha('9')] = true;
  nerdle->wrong[nerdle_map_alpha('2')][0] = true;
  nerdle->wrong[nerdle_map_alpha('3')][1] = true;
  nerdle->wrong[nerdle_map_alpha('=')][5] = true;
  nerdle->wrong[nerdle_map_alpha('4')][6] = true;

  nerdle->right[0] = '3';
  nerdle->right[4] = '=';
  nerdle->right[5] = '2';
  nerdle->discarded[nerdle_map_alpha('0')] = true;
  nerdle->wrong[nerdle_map_alpha('4')][3] = true;
  nerdle->wrong[nerdle_map_alpha('6')][6] = true;

  /* nerdle->right[5] = '8'; */
  /* nerdle->discarded[nerdle_map_alpha('0')] = true; */
  /* nerdle->discarded[nerdle_map_alpha('1')] = true; */
  /* nerdle->discarded[nerdle_map_alpha('-')] = true; */
  /* nerdle->wrong[nerdle_map_alpha('2')][3] = true; */
  /* nerdle->wrong[nerdle_map_alpha('=')][4] = true; */

  /* nerdle->discarded[nerdle_map_alpha('3')] = true; */
  /* nerdle->wrong[nerdle_map_alpha('2')][0] = true; */
  /* nerdle->wrong[nerdle_map_alpha('4')][1] = true; */
  /* nerdle->wrong[nerdle_map_alpha('=')][2] = true; */
  /* nerdle->wrong[nerdle_map_alpha('*')][4] = true; */

  /* nerdle->right[1] = '*'; */
  /* nerdle->right[3] = '='; */
  /* nerdle->right[4] = '2'; */
  /* nerdle->wrong[nerdle_map_alpha('4')][0] = true; */
  /* nerdle->wrong[nerdle_map_alpha('7')][2] = true; */

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
