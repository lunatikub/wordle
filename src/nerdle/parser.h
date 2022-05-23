#ifndef __PARSER__
#define __PARSER__

#include "lexer.h"
#include "equation.h"

/**
 * Parse and return an equation (not checked sementically).
 * If the string is not valid return NULL.
 */
struct equation* parse(const char *str, unsigned len);

#endif /* !__PARSER__ */
