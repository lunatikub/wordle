#ifndef __COORD__
#define __COORD__

/* Coordinates of a location. */
struct coord {
  int x;
  int y;
};

/**
 * Set coordinates from inouts x and y.
 */
void coord_set(struct coord *c, int x, int y);

#endif /* !__COORD__ */
