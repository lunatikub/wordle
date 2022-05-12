#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <stdbool.h>
#include <stdlib.h>

#include "color.h"

/* Coordinates of a location. */
struct coord {
  int x;
  int y;
};

/* Set coordinates from inouts x and y. */
void coord_set(struct coord *c, int x, int y);

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
 * Try to find an existing pixel `color` from `from` coordinates.
 * Browse the image horizontally until limits by increment x coordinate.
 */
bool utils_x11_find_h_inc_from(struct utils_x11 *x11, struct coord *from,
                               struct coord *coord, const struct color *color);

/**
 * Try to find an existing pixel `color` from `from` coordinates.
 * Browse the image horizontally until limits by decrement x coordinate.
 */
bool utils_x11_find_h_dec_from(struct utils_x11 *x11, struct coord *from,
                               struct coord *coord, const struct color *color);

/**
 * Try to find an existing pixel `color` from `from` coordinates.
 * Browse the image verticaly until limits by increment y coordinate.
 */
bool utils_x11_find_v_inc_from(struct utils_x11 *x11, struct coord *from,
                               struct coord *coord, const struct color *color);

/**
 * Try to find an existing pixel `color` from `from` coordinates.
 * Browse the image verticaly until limits by decrement y coordinate.
 */
bool utils_x11_find_v_dec_from(struct utils_x11 *x11, struct coord *from,
                               struct coord *coord, const struct color *color);

/**
 * Asking user to set the focus on the screen and the coordinates of the mouse.
 */
void utils_x11_focus(struct utils_x11 *x11, struct coord *coord, unsigned waiting_time);
