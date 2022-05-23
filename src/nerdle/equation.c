#include <stdlib.h>

#include "equation.h"

struct equation* equation_new(void)
{
  struct equation *eq = calloc(1, sizeof(*eq));
  return eq;
}

struct node* equation_node_new(enum node_type type)
{
  struct node *node = calloc(1, sizeof(*node));
  node->type = type;
  return node;
}

void equation_node_insert_tail(struct equation *eq, struct node *node)
{
  if (eq->head == NULL) {
    eq->head = node;
  }
  if (eq->tail != NULL) {
    eq->tail->next = node;
    node->prev = eq->tail;
  }
  eq->tail = node;
}

void equation_free(struct equation *eq)
{
  struct node *current = eq->head;

  while (current != NULL) {
    struct node *to_free = current;
    current = current->next;
    free(to_free);
  }
  free(eq);
}

static bool equation_once_operator_equal(const struct equation *eq)
{
  unsigned count_eq = 0;
  struct node *node = eq->head;

  while (node != NULL) {
    if (node->type == NODE_OPERATOR && node->operator == OPERATOR_EQ) {
      ++count_eq;
    }
    node = node->next;
  }
  return count_eq == 1;
}

static bool equation_has_sussessive_operators(const struct equation *eq)
{
  bool sussessive = 0;
  struct node *node = eq->head;

  while (node != NULL) {
    if (node->type == NODE_OPERATOR) {
      if (sussessive == true) {
        return true;
      }
      sussessive = true;
    } else {
      sussessive = false;
    }
    node = node->next;
  }
  return false;;
}

bool equation_is_valid(const struct equation *eq)
{
  /* must end with an operand */
  if (eq->tail->type != NODE_OPERAND) {
    return false;
  }

  /* must contain an operator `equal`. */
  if (equation_once_operator_equal(eq) == false) {
    return false;
  }

  /* cannot have two or more successive operators. */
  if (equation_has_sussessive_operators(eq) == true) {
    return false;
  }

  return true;
}
