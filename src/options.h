#ifndef __OPTIONS__
#define __OPTIONS__

#include "lang.h"

struct options {
  unsigned len;
  enum lang lang;
};

/**
 * Default values for the options. (if not set).
 */

#define DEFAULT_LEN 5
#define DEFAULT_LANG FR

/**
 * Parse the options:
 *  + language
 *  + lenght
 * The options are defined in the file `options.c`.
 */
void options_parse(int argc, char **argv, struct options *opts);

#endif /* !__OPTIONS__ */
