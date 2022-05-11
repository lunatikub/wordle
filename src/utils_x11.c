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
#define TIME_50MS 50000
  XTestFakeKeyEvent(x11->display, keycode, true, 0);
  XFlush(x11->display);
  usleep(TIME_50MS);
  XTestFakeKeyEvent(x11->display, keycode, false, 0);
  XFlush(x11->display);
#undef TIME_50MS
}

void utils_x11_write(struct utils_x11 *x11, const char *word, unsigned len)
{
  char letter[2];

  for (unsigned i = 0; i < len; ++i) {
    letter[0] = word[i];
    letter[1] = 0;
    KeyCode keycode = XKeysymToKeycode(x11->display, XStringToKeysym(letter));
    utils_x11_press_key(x11, keycode);
  }
  utils_x11_press_key(x11, 36); // return
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

bool utils_x11_find_h_from(struct utils_x11 *x11, struct coord *from,
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

bool utils_x11_find_v_from(struct utils_x11 *x11, struct coord *from,
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

void coord_set(struct coord *c, int x, int y)
{
  c->x = x;
  c->y = y;
}

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
