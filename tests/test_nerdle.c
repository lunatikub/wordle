#include "nerdle/nerdle.h"
#include "nerdle/lexer.h"

#include "nerdle/parser.h"
#include "test.h"

#define X NODE_OPERAND
#define Y NODE_OPERATOR

#define P OPERATOR_PLUS
#define M OPERATOR_MINUS
#define D OPERATOR_DIV
#define T OPERATOR_TIME
#define E OPERATOR_EQ

TEST_F(nerdle, lexer)
{
  struct token token;

#define EXPECT_TOKEN(STR, TYPE, END)                    \
  token.off_start = 0;                                  \
  token.off_end = 0;                                    \
  EXPECT_TRUE(lexer_token_fill((STR),                   \
                               strlen((STR)), &token)); \
  EXPECT_TRUE(token.type == (TYPE));                    \
  lexer_token_eat(&token);                              \
  EXPECT_TRUE(token.off_end == END);

  EXPECT_TOKEN("1", TOKEN_OPERAND, 1);
  EXPECT_TOKEN("12", TOKEN_OPERAND, 2);
  EXPECT_TOKEN("12+1", TOKEN_OPERAND, 2);
  EXPECT_TOKEN("+", TOKEN_OPERATOR_PLUS, 1);
  EXPECT_TOKEN("-", TOKEN_OPERATOR_MINUS, 1);
  EXPECT_TOKEN("/", TOKEN_OPERATOR_DIV, 1);
  EXPECT_TOKEN("*", TOKEN_OPERATOR_TIME, 1);
  EXPECT_TOKEN("=", TOKEN_OPERATOR_EQ, 1);

#undef EXPECT_TOKEN

  return true;
}

static bool parse_expected(const struct equation *eq,
                           unsigned nr,
                           const enum node_type *types,
                           const unsigned *operands,
                           const enum operator *operators)
{
  EXPECT_TRUE(eq != NULL);
  struct node *current = eq->head;

  for (unsigned i = 0; i < nr; ++i) {
    if (current == NULL) {
      return false;
    }
    if (types[i] != current->type) {
      return false;
    }
    if (current->type == X) {
      if (operands[i] != current->operand) {
        return false;
      }
    } else {
      if (operators[i] != current->operator) {
        return false;
      }
    }
    current = current->next;
  }
  return true;
}

TEST_F(nerdle, parser)
{
#define EXPECT_PARSE(STR, TYPES, OPERANDS, OPERATORS)                   \
  ({                                                                    \
    struct equation *eq = parse(STR, sizeof(STR) - 1);                  \
    unsigned len = sizeof(TYPES) / sizeof(enum node_type);              \
    EXPECT_TRUE(parse_expected(eq, len, TYPES, OPERANDS, OPERATORS));   \
    equation_free(eq);                                                  \
  })

  {
    enum node_type types[] = { X, Y, X, Y, X };
    unsigned operands[] = { 1, 0, 2, 0, 3 };
    enum operator operators[] = { 0, P, 0, E, 0 };
    EXPECT_PARSE("1+2=3", types, operands, operators);
  }
  {
    enum node_type types[] = { X, Y, X, Y, X };
    unsigned operands[] = { 12, 0, 22, 0, 33 };
    enum operator operators[] = { 0, T, 0, M, 0 };
    EXPECT_PARSE("12*22-33", types, operands, operators);
  }

#undef EXPECT_PARSE
  return true;
}

TEST_F(nerdle, is_valid)
{
#define EXPECT_IS_VALID(STR, RES)                       \
  ({                                                    \
    struct equation *eq = parse(STR, sizeof(STR) - 1);  \
    EXPECT_TRUE(equation_is_valid(eq) == RES);          \
    equation_free(eq);                                  \
  })

  EXPECT_IS_VALID("1+2=3", true);
  EXPECT_IS_VALID("12+22*3", false); /* missing '=' */
  EXPECT_IS_VALID("1+2=3+", false); /* end with an operator */
  EXPECT_IS_VALID("1++2", false); /* two sussessive operators */
  EXPECT_IS_VALID("+1+1=2", true);

#undef EXPECT_IS_VALID

  return true;
}

TEST_F(nerdle, reduce)
{
#define EXPECT_REDUCE(STR, RES) \
  ({                                                    \
    struct equation *eq = parse(STR, sizeof(STR) - 1);  \
    EXPECT_TRUE(equation_reduce(eq) == RES);            \
    equation_free(eq);                                  \
  })

  EXPECT_REDUCE("1+2=3", true);
  EXPECT_REDUCE("1+2*5=11", true);
  EXPECT_REDUCE("1-2+1*2=1", true);
  EXPECT_REDUCE("1-2+1*2=0", false);
  EXPECT_REDUCE("2/2+1*2=3", true);
  EXPECT_REDUCE("2/2*2+1*2=4", true);

#undef EXPECT_REDUCE
  return true;
}

#undef X
#undef Y

#undef P
#undef M
#undef D
#undef M
#undef E

const static struct test nerdle_tests[] = {
  TEST(nerdle, lexer),
  TEST(nerdle, parser),
  TEST(nerdle, is_valid),
  TEST(nerdle, reduce),
};

TEST_SUITE(nerdle);
