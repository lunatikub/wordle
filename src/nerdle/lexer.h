#ifndef __LEXER__
#define __LEXER__

#include <stdbool.h>

enum token_type {
  TOKEN_OPERAND,
  TOKEN_OPERATOR_PLUS,
  TOKEN_OPERATOR_MINUS,
  TOKEN_OPERATOR_DIV,
  TOKEN_OPERATOR_TIME,
  TOKEN_OPERATOR_EQ,
  TOKEN_END,
};

struct token {
  enum token_type type;
  unsigned off_start;
  unsigned off_end;
};

/**
 * Fill the next token.
 */
bool lexer_token_fill(const char *eq, unsigned len, struct token *token);

/**
 * Eat a token.
 */
void lexer_token_eat(struct token *token);

#endif /* !__LEXER__ */
