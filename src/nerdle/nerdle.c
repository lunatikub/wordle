#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "nerdle.h"
#include "equation.h"
#include "parser.h"

/* Alphabet of a nerdle equation. */
static const char alpha[] = "0123456789+-/*=";

/* Map a char of the alphabet on an index.
   Index is the position in the alphabet. */
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

/**
 * Create a new candidate for the equation.
 */
static struct candidate* nerdle_candidate_new(const char *equation)
{
  struct candidate *candidate = calloc(1, sizeof(*candidate));
  candidate->equation = strdup(equation);
  return candidate;
}

/**
 * Insert a new candidate at the head of the candidates list.
 */
static void nerdle_candidate_insert_head(struct nerdle *nerdle, struct candidate *candidate)
{
  candidate->next = nerdle->candidates;
  if (nerdle->candidates != NULL) {
    nerdle->candidates->prev = candidate;
  }
  nerdle->candidates = candidate;
  ++nerdle->nr_candidate;
}

/**
 * Remove a candidate from the candidates list.
 */
static void nerdle_candidate_remove(struct nerdle *nerdle, struct candidate *candidate)
{
  if (candidate->prev == NULL) { /* head */
    nerdle->candidates = candidate->next;
  } else {
    candidate->prev->next = candidate->next;
  }
  if (candidate->next != NULL) {
    candidate->next->prev = candidate->prev;
  }
  free(candidate->equation);
  free(candidate);
  --nerdle->nr_candidate;
}

/**
 * Check if the character at the position is valid.
 */
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
  ++nerdle->nr_call;
  if (position == nerdle->len) {
    struct equation *eq = parse(str, nerdle->len);
    if (eq == NULL) {
      ++nerdle->nr_fail;
      return;
    }
    if (equation_is_valid(eq) == true && equation_reduce(eq) == true) {
      struct candidate *candidate = nerdle_candidate_new(str);
      nerdle_candidate_insert_head(nerdle, candidate);
    } else {
      ++nerdle->nr_fail;
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

  /* optimization: don't try the branchs starting by '/+-*=' */
  for (unsigned i = 0; i < 10; ++i) {
    if (nerdle_is_valid_char(nerdle, alpha[i], 0) == false) {
      continue;
    }
    str[0] = alpha[i];
    nerdle_generate_equations_rec(nerdle, str, 1);
  }
}

static struct candidate*
nerdle_remove_candidate_if_needed(struct nerdle *nerdle, struct candidate *candidate)
{
  for (unsigned i = 0; i < nerdle->len; ++i) {
    if (nerdle_is_valid_char(nerdle, candidate->equation[i], i) == false) {
      struct candidate *next = candidate->next;
      nerdle_candidate_remove(nerdle, candidate);
      return next;
    }
  }
  return candidate->next;
}

const char* nerdle_first_equation(struct nerdle *nerdle)
{
  switch (nerdle->len) {
    case 5:  return "1+2=3";
    case 6:  return "10-2=8";
    case 7:  return "23-19=4";
    case 8:  return "9+8-3=14";
    case 9:  return "20+3-8=15";
    default:
      assert(!"TODO");
  }
}

void nerdle_select_equations(struct nerdle *nerdle)
{
  struct candidate *candidate = nerdle->candidates;
  while (candidate != NULL) {
    candidate = nerdle_remove_candidate_if_needed(nerdle, candidate);
  }
}

static unsigned nerdle_get_variance(struct nerdle *nerdle, const char *equation)
{
  unsigned variance = 0;
  bool once[ALPHA_SZ] = { false };

  for (unsigned i = 0; i < nerdle->len; ++i) {
    unsigned mapped = nerdle_map_alpha(equation[i]);
    if (once[mapped] == false) {
      once[mapped] = true;
      ++variance;
    }
  }

  return variance;
}

const char* nerdle_find_best_equation(struct nerdle *nerdle)
{
  if (nerdle->nr_candidate == 0) {
    return NULL;
  }

  struct candidate *candidate = nerdle->candidates;
  const char *best_candidate = nerdle->candidates->equation;
  unsigned best_variance = 0;

  while (candidate != NULL) {
    unsigned variance = nerdle_get_variance(nerdle, candidate->equation);
    if (variance > best_variance) {
      best_variance = variance;
      best_candidate = candidate->equation;
    }
    candidate = candidate->next;
  }

  return best_candidate;
}

struct nerdle* nerdle_create(unsigned len)
{
  assert(len >= MIN_EQ_SZ && len <= MAX_EQ_SZ);

  struct nerdle *nerdle = calloc(1, sizeof(*nerdle));
  nerdle->len = len;
  return nerdle;
}

void nerdle_destroy(struct nerdle *nerdle)
{
  struct candidate *candidate = nerdle->candidates;

  while (candidate != NULL) {
    struct candidate *next = candidate->next;
    free(candidate->equation);
    free(candidate);
    candidate = next;
  }

  free(nerdle);
}
