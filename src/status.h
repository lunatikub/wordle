#ifndef __STATUS__
#define __STATUS__

#include "color.h"

enum status {
  UNKNOWN,
  DISCARDED,
  WRONG,
  RIGHT,
};

/**
 * Dump in color a status.
 */
void status_dump(enum status status);

/**
 * Map a status from the color of the differents status.
 */
enum status status_map_from_colors(struct color *color,
                                   const struct color *c_right,
                                   const struct color *c_wrong,
                                   const struct color *c_discarded);

#endif /* !__STATUS__ */
