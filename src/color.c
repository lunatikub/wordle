#include <stdlib.h>

#include "color.h"

bool color_approx_eq(const struct color *c1, const struct color *c2)
{
#define LOCAL_APPROX 10
#define TOTAL_APPROX 20

  int diff_r = abs(c1->r - c2->r);
  int diff_g = abs(c1->g - c2->g);
  int diff_b = abs(c1->b - c2->b);

  if (diff_r > LOCAL_APPROX ||
      diff_g > LOCAL_APPROX ||
      diff_b > LOCAL_APPROX) {
    return false;
  }

  return (diff_r + diff_g + diff_b) < TOTAL_APPROX;

#undef LOCAL_APPROX
#undef TOTAL_APPROX
}
