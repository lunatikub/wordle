#ifndef __NERDLE__
#define __NERDLE__

#include <stdbool.h>
#include <sys/queue.h>

#define MIN_EQ_SZ 5
#define MAX_EQ_SZ 12

#define ALPHA_SZ 15 /* 0123456789/+-*= */

struct candidate {
  char *equation;
  bool eliminated;
  STAILQ_ENTRY(candidate) next;
};

struct nerdle {
  unsigned len;
  char right[MAX_EQ_SZ];
  bool discarded[ALPHA_SZ];
  bool wrong[ALPHA_SZ][MAX_EQ_SZ];
  STAILQ_HEAD(, candidate) candidates;
  unsigned nr_candidate;
};

struct nerdle* nerdle_create(unsigned len);

void nerdle_destroy(struct nerdle *nerdle);

void nerdle_generate_equations(struct nerdle *nerdle);

const char* nerdle_find_best_equation(struct nerdle *nerdle);

const char *nerdle_first_equation(struct nerdle *nerdle);

unsigned nerdle_map_alpha(char c);

void nerdle_eliminate_candidates(struct nerdle *nerlde);

#endif /* !__NERDLE__ */