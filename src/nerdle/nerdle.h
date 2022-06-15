#ifndef __NERDLE__
#define __NERDLE__

#include <stdbool.h>

#define MIN_EQ_SZ 5
#define MAX_EQ_SZ 12

#define ALPHA_SZ 15 /* 0123456789/+-*= */

struct candidate {
  char *equation;
  struct candidate *next;
  struct candidate *prev;
};

struct nerdle {
  unsigned len;
  char right[MAX_EQ_SZ];
  bool discarded[ALPHA_SZ];
  bool wrong[ALPHA_SZ][MAX_EQ_SZ];
  struct candidate *candidates;
  unsigned nr_candidate;
};

/**
 * Create a nerdle IA.
 */
struct nerdle* nerdle_create(unsigned len);

/**
 * Destroy a nerdle IA previously created.
 */
void nerdle_destroy(struct nerdle *nerdle);

/**
 * TODO: merge with select.
 */
const char *nerdle_first_equation(struct nerdle *nerdle);

/**
 * Select the best equation to play.
 * TODO: add round.
 */
void nerdle_select_equations(struct nerdle *nerlde);

/**
 * Generate the list of candidate equations from the first input.
 */
void nerdle_generate_equations(struct nerdle *nerdle);

/* TODO will be removed */
const char* nerdle_find_best_equation(struct nerdle *nerdle);
unsigned nerdle_map_alpha(char c);

#endif /* !__NERDLE__ */
