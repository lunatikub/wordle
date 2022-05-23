#include "lexer.h"

#define OP_PLUS '+'
#define OP_MINUS '-'
#define OP_DIV '/'
#define OP_TIME '*'
#define OP_EQ '='

#define IS_DIGIT(X) ((X) >= '0' && (X) <= '9')

static void lexer_operand(const char *eq, unsigned len, struct token *token)
{
  while (token->off_end < len && IS_DIGIT(eq[token->off_end])) {
    ++token->off_end;
  }
  token->type = TOKEN_OPERAND;
}

bool lexer_token_fill(const char *eq, unsigned len, struct token *token)
{
  if (len == token->off_start) {
    token->type = TOKEN_END;
    return true;
  }

  if (IS_DIGIT(eq[token->off_start]) == true) {
    lexer_operand(eq, len, token);
    return true;
  }

  switch (eq[token->off_start]) {
    case OP_PLUS:
      token->type = TOKEN_OPERATOR_PLUS;
      break;
    case OP_MINUS:
      token->type = TOKEN_OPERATOR_MINUS;
      break;
    case OP_DIV:
      token->type = TOKEN_OPERATOR_DIV;
      break;
    case OP_TIME:
      token->type = TOKEN_OPERATOR_TIME;
      break;
    case OP_EQ:
      token->type = TOKEN_OPERATOR_EQ;
      break;
    default:
      return false;
  };
  ++token->off_end;

  return true;
}

void lexer_token_eat(struct token *token)
{
  token->off_start = token->off_end;
}
