#include <assert.h>
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
  ++eq->nr;
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

bool equation_is_valid(struct equation *eq)
{
  /* must end with an operand */
  if (eq->tail->type != NODE_OPERAND) {
    return false;
  }

  struct node *node = eq->head;
  bool successive = false;
  unsigned successive_operator = 0;
  unsigned nr_operator = 0; /* =/+*- */
  unsigned nr_equal = 0; /* = */

  while (node != NULL) {
    if (node->type == NODE_OPERATOR) {
      ++nr_operator;
      if (node->operator == OPERATOR_EQ) {
        ++nr_equal;
      }
      ++successive_operator;
      if (successive_operator > 1) {
        successive = true;
      }
    } else {
      successive_operator = 0;
    }
    node = node->next;
  }

  return successive == false && nr_operator >= 2 && nr_equal == 1;
}

/**
 * Div by 0 is forbidden.
 * Only allows integer division.
 */
static bool equation_check_div(struct node *node,
                               struct node *left,
                               struct node *right)
{
  if (node->operator != OPERATOR_DIV) {
    return true;
  }

  if (right->operand == 0) {
    return false;
  }

  if ((left->operand / right->operand) * right->operand != left->operand) {
    return false;
  }

  return true;
}

static bool equation_nodes_merge(struct equation *eq,
                                 struct node *left,
                                 struct node *node,
                                 struct node *right)
{
  if (equation_check_div(node, left, right) == false) {
    return false;
  }

  node->prev = left->prev;
  node->next = right->next;

  if (left->prev != NULL) {
    left->prev->next = node;
  } else {
    eq->head = node;
  }

  if (right->next != NULL) {
    right->next->prev = node;
  } else {
    eq->tail = node;
  }

  switch (node->operator) {
    case OPERATOR_TIME:
      node->operand = left->operand * right->operand;
      break;
    case OPERATOR_DIV:
      node->operand = left->operand / right->operand;
      break;
    case OPERATOR_PLUS:
      node->operand = left->operand + right->operand;
      break;
    case OPERATOR_MINUS:
      node->operand = left->operand - right->operand;
      break;
    case OPERATOR_EQ:
      break;
  };

  node->type = NODE_OPERAND;

  free(left);
  free(right);
  eq->nr -= 2;
  return true;
}

static bool equation_reduce_operator(struct equation *eq, enum operator operator)
{
  struct node *node = eq->head;

  while (node != NULL) {
    if (node->type == NODE_OPERATOR && node->operator == operator) {
      struct node *left = node->prev;
      struct node *right = node->next;
      if (left == NULL || left->type != NODE_OPERAND ||
          right == NULL || right->type != NODE_OPERAND) {
        return false;
      }
      if (equation_nodes_merge(eq, left, node, right) == false) {
        return false;
      }
    }
    node = node->next;
  }
  return true;
}

bool equation_reduce(struct equation *eq)
{
  if (equation_reduce_operator(eq, OPERATOR_DIV) == false) {
    return false;
  }
  if (equation_reduce_operator(eq, OPERATOR_TIME) == false) {
    return false;
  }
  if (equation_reduce_operator(eq, OPERATOR_MINUS) == false) {
    return false;
  }
  if (equation_reduce_operator(eq, OPERATOR_PLUS) == false) {
    return false;
  }

  /* operand = operand */
  if (eq->nr != 3 ||
      eq->head->type != NODE_OPERAND ||
      eq->tail->type != NODE_OPERAND ||
      eq->head->next->type != NODE_OPERATOR ||
      eq->head->next->operator != OPERATOR_EQ ||
      eq->head->operand != eq->tail->operand) {
    return false;
  }

  return true;
}
