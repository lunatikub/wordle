#include <stdio.h>
#include <assert.h>

#include "status.h"

void status_dump(enum status status)
{
  switch (status) {
    case RIGHT:
      printf("\033[0;32mO");
      break;
    case WRONG:
      printf("\033[0;33mX");
      break;
    case DISCARDED:
      printf("_");
      break;
    default:
      assert(!"location name not found...");
  };
  printf("\033[0m");
}

enum status status_map_from_colors(struct color *color,
                                   const struct color *c_right,
                                   const struct color *c_wrong,
                                   const struct color *c_discarded)
{
  if (color_approx_eq(color, c_right) == true) {
    return RIGHT;
  }
  if (color_approx_eq(color, c_wrong) == true) {
    return WRONG;
  }
  if (color_approx_eq(color, c_discarded) == true) {
    return DISCARDED;
  }
  return UNKNOWN;
}
