#ifndef __OPTIONS__
#define __OPTIONS__

#include "lang.h"

struct options {
  unsigned len;
  enum lang lang;
  char *first;
};

/**
 * Default values for the options. (if not set).
 */

#define DEFAULT_LEN 5
#define DEFAULT_LANG FR
#define DEFAULT_FIRST NULL

/**
 * Parse the options:
 *  + language
 *  + lenght
 * The options are defined in the file `options.c`.
 */
void options_parse(int argc, char **argv, struct options *opts);

/**
 * Clean the options previously parsed.
 */
void options_clean(struct options *opts);

#endif /* !__OPTIONS__ */
