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

struct nerdle* nerdle_create(unsigned len);

void nerdle_destroy(struct nerdle *nerdle);

void nerdle_generate_equations(struct nerdle *nerdle);

const char* nerdle_find_best_equation(struct nerdle *nerdle);

const char *nerdle_first_equation(struct nerdle *nerdle);

unsigned nerdle_map_alpha(char c);

void nerdle_select_equations(struct nerdle *nerlde);

#endif /* !__NERDLE__ */
