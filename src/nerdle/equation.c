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

bool equation_is_valid(struct equation *eq)
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

static void equation_nodes_merge(struct equation *eq,
                                 struct node *left,
                                 struct node *node,
                                 struct node *right)
{
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
      equation_nodes_merge(eq, left, node, right);
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
