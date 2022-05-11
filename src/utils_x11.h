#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <stdbool.h>
#include <stdlib.h>

/* Coordinates of a location. */
struct coord {
  int x;
  int y;
};

/* Set coordinates from inouts x and y. */
void coord_set(struct coord *c, int x, int y);

/* RGB */
struct color {
  int r;
  int g;
  int b;
};

/**
 * Return true if approximalty the both colors are the same.
 * Otherwise return false.
 **/
bool color_approx_eq(const struct color *c1, const struct color *c2);

struct utils_x11 {
  Display *display;
  Window win;
  int screen;
  Colormap map;
  XImage *image;
  unsigned width;
  unsigned height;
};

/**
 * Initialize x11 to get the current screen, the current window,
 * the color map, the width and the height.
 */
bool utils_x11_init(struct utils_x11 *x11);

/**
 * Refresh the image of the screen.
 */
void utils_x11_image_refresh(struct utils_x11 *x11);

/**
 * Get the coordiantes of the mouse.
 */
struct coord utils_x11_get_mouse_coordinates(struct utils_x11 *x11);

/**
 * Get the color of a pixel from coordinates.
 */
void utils_x11_color_get(struct utils_x11 *x11, int x, int y, struct color *color);

/**
 * Write a word with the keyboard.
 **/
void utils_x11_write(struct utils_x11 *x11, const char *word, unsigned len);

/**
 * Try to find an existing `color` from `from` coordinates.
 * If found return with output `coord`.
 * Browse the image horizontally then vertically untils limits.
 */
bool utils_x11_find_from(struct utils_x11 *x11, struct coord *from,
                         struct coord *coord, const struct color *color);

/**
 * Try to find an existing `color` from `from` coordinates.
 * If found return with output `coord`.
 * Browse the image horizontally until limits.
 */
bool utils_x11_find_h_from(struct utils_x11 *x11, struct coord *from,
                           struct coord *coord, const struct color *color);

/**
 * Try to find an existing `color` from `from` coordinates.
 * If found return with output `coord`.
 * Browse the image verticaly until limits.
 */
bool utils_x11_find_v_from(struct utils_x11 *x11, struct coord *from,
                           struct coord *coord, const struct color *color);
