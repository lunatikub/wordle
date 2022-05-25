#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "nerdle.h"
#include "equation.h"
#include "parser.h"

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
  return candidate;
}

static void nerdle_generate_equations_rec(struct nerdle *nerdle, char *str, unsigned idx)
{
  if (idx == nerdle->len) {
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
    if (nerdle->discarded[nerdle_map_alpha(alpha[i])] == true) {
      continue;
    }
    if (nerdle->right[idx] != 0 && nerdle->right[idx] != alpha[i]) {
      continue;
    }
    str[idx] = alpha[i];
    nerdle_generate_equations_rec(nerdle, str, idx + 1);
  }
}

void nerdle_generate_equations(struct nerdle *nerdle)
{
  char str[MAX_EQ_SZ] = { 0 };
  nerdle_generate_equations_rec(nerdle, str, 0);
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
