#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "nerdle.h"
#include "equation.h"
#include "parser.h"

/* alphabet of a nerdle equation. */
static const char alpha[] = "0123456789+-/*=";

unsigned nerdle_map_alpha(char c)
{
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  switch (c) {
    case '+':
      return 10;
    case '-':
      return 11;
    case '/':
      return 12;
    case '*':
      return 13;
  };
  return 14; /* = */
}

static struct candidate* nerdle_candidate_new(const char *equation)
{
  struct candidate *candidate = malloc(sizeof(*candidate));
  candidate->equation = strdup(equation);
  candidate->eliminated = false;
  return candidate;
}

static bool nerdle_is_valid_char(struct nerdle *nerdle, char c, unsigned position)
{
  unsigned mapped = nerdle_map_alpha(c);

  if (nerdle->discarded[mapped] == true) {
    return false;
  }
  if (nerdle->right[position] != 0 && nerdle->right[position] != c) {
    return false;
  }
  if (nerdle->wrong[mapped][position] == true) {
    return false;
  }
  return true;
}

static void nerdle_generate_equations_rec(struct nerdle *nerdle, char *str, unsigned position)
{
  if (position == nerdle->len) {
    struct equation *eq = parse(str, nerdle->len);
    if (eq == NULL) {
      return;
    }
    if (equation_is_valid(eq) == true && equation_reduce(eq) == true) {
      struct candidate *candidate = nerdle_candidate_new(str);
      STAILQ_INSERT_TAIL(&nerdle->candidates, candidate, next);
      ++nerdle->nr_candidate;
    }
    equation_free(eq);
    return;
  }

  for (unsigned i = 0; i < sizeof(alpha) - 1; ++i) {
    if (nerdle_is_valid_char(nerdle, alpha[i], position) == false) {
      continue;
    }
    str[position] = alpha[i];
    nerdle_generate_equations_rec(nerdle, str, position + 1);
  }
}

void nerdle_generate_equations(struct nerdle *nerdle)
{
  char str[MAX_EQ_SZ] = { 0 };
  nerdle_generate_equations_rec(nerdle, str, 0);
}

void nerdle_eliminate_candidates(struct nerdle *nerdle)
{
  struct candidate *candidate;
  STAILQ_FOREACH(candidate, &nerdle->candidates, next) {
    if (candidate->eliminated == true) {
      continue;
    }
    for (unsigned i = 0; i < nerdle->len; ++i) {
      if (nerdle_is_valid_char(nerdle, candidate->equation[i], i) == false) {
        candidate->eliminated = true;
        --nerdle->nr_candidate;
        break;
      }
    }
  }
}

const char* nerdle_find_best_equation(struct nerdle *nerdle)
{
  if (nerdle->nr_candidate == 0) {
    return NULL;
  }

  struct candidate *candidate;
  const char *best_candidate = STAILQ_FIRST(&nerdle->candidates)->equation;
  unsigned best_variance = 0;

  STAILQ_FOREACH(candidate, &nerdle->candidates, next) {
    if (candidate->eliminated == true) {
      continue;
    }
    bool alpha_once[sizeof(alpha) / sizeof(char)] = { false };
    unsigned variance = 0;
    for (unsigned i = 0; i < nerdle->len; ++i) {
      unsigned idx = nerdle_map_alpha(candidate->equation[i]);
      if (alpha_once[idx] == false) {
        alpha_once[idx] = true;
        ++variance;
      }
    }
    if (variance > best_variance) {
      best_variance = variance;
      best_candidate = candidate->equation;
    }
  }

  return best_candidate;
}

const char* nerdle_first_equation(struct nerdle *nerdle)
{
  switch (nerdle->len) {
    case 5:  return "1+2=3";
    case 6:  return "10-2=8";
    case 7:  return "23-19=4";
    case 8:  return "9+8-3=14";
    case 9:  return "20+3-8=15";
  }
  return "TODO"; /* 10, 11, 12 */
}

struct nerdle* nerdle_create(unsigned len)
{
  assert(len >= MIN_EQ_SZ && len <= MAX_EQ_SZ);

  struct nerdle *nerdle = calloc(1, sizeof(*nerdle));
  nerdle->len = len;
  STAILQ_INIT(&nerdle->candidates);
  return nerdle;
}

void nerdle_destroy(struct nerdle *nerdle)
{
  struct candidate *candidate = NULL;

  while (!STAILQ_EMPTY(&nerdle->candidates)) {
    candidate = STAILQ_FIRST(&nerdle->candidates);
    STAILQ_REMOVE_HEAD(&nerdle->candidates, next);
    free(candidate->equation);
    free(candidate);
  }

  free(nerdle);
}
