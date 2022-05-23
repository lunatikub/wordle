#include <stdlib.h>
#include <assert.h>

#include "parser.h"

static unsigned token_operand_to_unsigned(struct token *token, const char *str)
{
  const char *start = str + token->off_start;
  const char *end = str + token->off_end;

  const char *iter = NULL;
  unsigned i = 0;

  for (iter = start; iter != end; ++iter) {
    i = i * 10 + (*iter - '0');
  }
  return i;
}

static struct node* parse_operand(const char *str, struct token *token)
{
  struct node *node;

  node = equation_node_new(NODE_OPERAND);
  node->operand = token_operand_to_unsigned(token, str);

  return node;
}

static struct node* parse_operator(struct token *token)
{
  struct node *node;

  node = equation_node_new(NODE_OPERATOR);

  switch (token->type) {
    case TOKEN_OPERATOR_PLUS:
      node->operator = OPERATOR_PLUS;
      break;
    case TOKEN_OPERATOR_MINUS:
      node->operator = OPERATOR_MINUS;
      break;
    case TOKEN_OPERATOR_DIV:
      node->operator = OPERATOR_DIV;
      break;
    case TOKEN_OPERATOR_TIME:
      node->operator = OPERATOR_TIME;
      break;
    case TOKEN_OPERATOR_EQ:
      node->operator = OPERATOR_EQ;
      break;
    default:
      assert(!"not a valid operator");
  }
  return node;
}

struct equation* parse(const char *str, unsigned len)
{
  struct equation *eq = equation_new();
  struct node *node;
  struct token token = { 0 };

  while (true) {
    if (lexer_token_fill(str, len, &token) == false) {
      return NULL;
    }
    if (token.type == TOKEN_END) {
      break;
    } else if (token.type == TOKEN_OPERAND) {
      node = parse_operand(str, &token);
    } else {
      node = parse_operator(&token);
    }
    if (node != NULL) {
      equation_node_insert_tail(eq, node);
    }
    lexer_token_eat(&token);
  }

  return eq;
}
