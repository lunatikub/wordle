#ifndef __COLOR__
#define __COLOR__

#include <stdbool.h>

/* RGB */
struct color {
  int r;
  int g;
  int b;
};

/* Cannot find a color on the screen. */
#define CANNOT_FIND_COLOR(COLOR)                                        \
  fprintf(stderr, "[color] cannot find color: (%i, %i, %i)", COLOR.r, COLOR.g, COLOR.b);

/**
 * Return true if approximalty the both colors are the same.
 * Otherwise return false.
 **/
bool color_approx_eq(const struct color *c1, const struct color *c2);

#endif /* !__COLOR__ */
