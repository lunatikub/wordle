#ifndef __EQUATION__
#define __EQUATION__

#include <stdbool.h>

enum node_type {
  NODE_OPERATOR,
  NODE_OPERAND,
};

enum operator {
  OPERATOR_PLUS,
  OPERATOR_MINUS,
  OPERATOR_DIV,
  OPERATOR_TIME,
  OPERATOR_EQ,
};

struct node {
  enum node_type type;
  union {
    unsigned operand;
    enum operator operator;
  };
  struct node *next;
  struct node *prev;
};

struct equation {
  struct node *head;
  struct node *tail;
};

/**
 * Create a new equation.
 */
struct equation* equation_new(void);

/**
 * Create a new node.
 */
struct node* equation_node_new(enum node_type type);

/**
 * Insert a node at the tail of the equation.
 */
void equation_node_insert_tail(struct equation *eq, struct node *node);

/**
 * Free an equation previously allocated with `equation_new`.
 */
void equation_free(struct equation *eq);

/**
 * Check if an equation is valid.
 */
bool equation_is_valid(const struct equation *eq);

#endif /* !__EQUATION__ */
