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

static void inputs(int *status)
{
  printf("status ? ");
  int n = fscanf(stdin, "%u %u %u %u %u %u %u %u",
                 &status[0], &status[1], &status[2], &status[3],
                 &status[4], &status[5], &status[6], &status[7]);
  printf("status: %u (%u %u %u %u %u %u %u %u)\n", n,
         status[0], status[1], status[2], status[3],
         status[4], status[5], status[6], status[7]);
}

static void update(struct nerdle *nerdle, int *status, const char *equation)
{
  for (unsigned i = 0; i < nerdle->len; ++i) {
    unsigned mapped = nerdle_map_alpha(equation[i]);
    if (status[i] == 0) {
      nerdle->right[i] = equation[i];
    } else if (status[i] == 1) {
      nerdle->wrong[mapped][i] = true;
    } else {
      nerdle->discarded[mapped] = true;
    }
  }
}

#define LEN 8

int main(void)
{
  (void)debug;

  printf("status 0: Right\n");
  printf("status 1: Wrong\n");
  printf("status 2: Discarded\n");
  printf("\n");

  struct nerdle *nerdle = nerdle_create(LEN);
  int status[LEN];
  const char *equation = nerdle_first_equation(nerdle);

  printf("first equation: %s\n", equation);
  inputs(status);
  update(nerdle, status, equation);
  nerdle_generate_equations(nerdle);

  for (unsigned round = 0; round < 5; ++round) {
    printf("nr_candidate: %u\n", nerdle->nr_candidate);
    equation = nerdle_find_best_equation(nerdle);
    printf("round: %u, next equation: %s\n", round, equation);
    inputs(status);
    update(nerdle, status, equation);
    nerdle_select_equations(nerdle);
  }

  nerdle_destroy(nerdle);

  return 0;
}
