#include <stdio.h>
#include <unistd.h>

#include "utils_x11.h"

void utils_x11_image_refresh(struct utils_x11 *x11)
{
  if (x11->image != NULL) {
    XFree(x11->image);
  }
  x11->image = XGetImage(x11->display, x11->win, 0, 0,
                         x11->width, x11->height, AllPlanes, XYPixmap);
}

bool utils_x11_init(struct utils_x11 *x11)
{
  x11->display = XOpenDisplay(NULL);
  if (x11->display == NULL) {
    return false;
  }

  x11->screen = DefaultScreen(x11->display);
  x11->win = RootWindow(x11->display, x11->screen);
  x11->map = DefaultColormap(x11->display, x11->screen);
  x11->width = DisplayWidth(x11->display, x11->screen);
  x11->height = DisplayHeight(x11->display, x11->screen);

  return true;
}

struct coord utils_x11_get_mouse_coordinates(struct utils_x11 *x11)
{
  struct coord coord;
  int childx, childy;
  unsigned int mask;
  Window w1, w2;

  XQueryPointer(x11->display, x11->win, &w1, &w2,
        	&coord.x, &coord.y, &childx, &childy, &mask);
  return coord;
}

void utils_x11_color_get(struct utils_x11 *x11, int x, int y, struct color *color)
{
  XColor xcolor;

  xcolor.pixel = XGetPixel(x11->image, x, y);
  XQueryColor(x11->display, x11->map, &xcolor);

  color->r = xcolor.red / 256;
  color->g = xcolor.green / 256;
  color->b = xcolor.blue / 256;
}

static void utils_x11_press_key(struct utils_x11 *x11, KeyCode keycode)
{
#define WAITING_TIME 30000 /* ms */
  XTestFakeKeyEvent(x11->display, keycode, true, 0);
  XFlush(x11->display);
  usleep(WAITING_TIME);
  XTestFakeKeyEvent(x11->display, keycode, false, 0);
  XFlush(x11->display);
#undef WAITING_TIME
}

void utils_x11_write(struct utils_x11 *x11, const char *word, unsigned len)
{
  char letter[2];
  KeyCode keycode;

#define CASE_KEYCODE(KEYCODE, LETTER)           \
  case LETTER:                                  \
    keycode = KEYCODE;                          \
    break

  for (unsigned i = 0; i < len; ++i) {
    switch (word[i]) {
      CASE_KEYCODE(86, '+');
      CASE_KEYCODE(21, '=');
      CASE_KEYCODE(82, '-');
      CASE_KEYCODE(106, '/');
      CASE_KEYCODE(63, '*');
      default:
        letter[0] = word[i];
        letter[1] = 0;
        keycode = XKeysymToKeycode(x11->display, XStringToKeysym(letter));
        break;
    };
    utils_x11_press_key(x11, keycode);
  }
  utils_x11_press_key(x11, 36); // return

#undef CASE_KEYCODE
}

bool utils_x11_find_from(struct utils_x11 *x11, struct coord *from,
                         struct coord *coord, const struct color *color)
{
  struct color current;

  for (unsigned y = from->y; y < x11->height; ++y) {
    for (unsigned x = from->x; x < x11->width; ++x) {
      utils_x11_color_get(x11, x, y, &current);
      if (color_approx_eq(&current, color) == true) {
        coord_set(coord, x, y);
        return true;
      }
    }
  }
  return false;
}

bool utils_x11_find_h_inc_from(struct utils_x11 *x11, struct coord *from,
                               struct coord *coord, const struct color *color)
{
  struct color current;
  for (unsigned x = from->x; x < x11->width; ++x) {
    utils_x11_color_get(x11, x, from->y, &current);
    if (color_approx_eq(&current, color) == true) {
      coord_set(coord, x, from->y);
      return true;
    }
  }
  return false;
}

bool utils_x11_find_h_dec_from(struct utils_x11 *x11, struct coord *from,
                               struct coord *coord, const struct color *color)
{
  struct color current;
  for (int x = from->x; x >= 0; --x) {
    utils_x11_color_get(x11, x, from->y, &current);
    if (color_approx_eq(&current, color) == true) {
      coord_set(coord, x, from->y);
      return true;
    }
  }
  return false;
}

bool utils_x11_find_v_inc_from(struct utils_x11 *x11, struct coord *from,
                               struct coord *coord, const struct color *color)
{
  struct color current;
  for (unsigned y = from->y; y < x11->height; ++y) {
    utils_x11_color_get(x11, from->x, y, &current);
    if (color_approx_eq(&current, color) == true) {
      coord_set(coord, from->x, y);
      return true;
    }
  }
  return false;
}

bool utils_x11_find_v_dec_from(struct utils_x11 *x11, struct coord *from,
                               struct coord *coord, const struct color *color)
{
  struct color current;
  for (int y = from->y; y >= 0; --y) {
    utils_x11_color_get(x11, from->x, y, &current);
    if (color_approx_eq(&current, color) == true) {
      coord_set(coord, from->x, y);
      return true;
    }
  }
  return false;
}

void utils_x11_focus(struct utils_x11 *x11, struct coord *coord,
                     unsigned waiting_time, const char *prefix)
{
  for (unsigned i = 0; i < waiting_time; ++i) {
    printf("[%s] %u/%u...\n", prefix, i + 1, waiting_time);
    sleep(1);
  }
  *coord = utils_x11_get_mouse_coordinates(x11);
  utils_x11_image_refresh(x11);
}
