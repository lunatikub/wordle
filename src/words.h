#ifndef __WORD__
#define __WORD__

#include <stdbool.h>

struct word {
  const char **list;
  unsigned nr;
  unsigned len;
};

/**
 * Check the list of words.
 * Word can only contains [a-z] characters.
 */
bool words_check_list(struct word *words);

#endif /* !__WORD__ */
