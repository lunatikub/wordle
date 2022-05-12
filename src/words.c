#include "words.h"

bool words_check_list(struct word *words)
{
  for (unsigned i = 0; i < words->nr; ++i) {
    for (unsigned j = 0; j < words->len; ++j) {
      char c = words->list[i][j];
      if (c < 'a' || c > 'z') {
        return false;
      }
    }
  }
  return true;
}
