#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "wordle.h"
#include "fr_words_5.h"

/* Bot interaction. */
#define BOT(fmt, ...) printf("[bot] " fmt "\n", ##__VA_ARGS__)

/* Error bot interaction. */
#define ERR(fmt, ...) fprintf(stderr, "[err] " fmt "...\n", ##__VA_ARGS__)

/* One of the best word to start with. */
#define FIRST_CANDIDATE "tarie"

/* RGB */
struct color {
  int r;
  int g;
  int b;
};

static bool color_eq(struct color *c1, struct color *c2)
{
  return c1->r == c2->r && c1->g == c2->g && c1->b == c2->b;
}

/* Coordinates of a location. */
struct coord {
  int x;
  int y;
};

static void coord_set(struct coord *c, int x, int y)
{
  c->x = x;
  c->y = y;
}

enum location {
  LOCATION_RIGHT,
  LOCATION_WRONG,
  LOCATION_DISCARDED,
  LOCATION_EMPTY,
  LOCATION_FIRST,
  LOCATION_END,
};

static void dump_location_status(enum location location)
{
  printf("\033[0;37m[");
  switch (location) {
    case LOCATION_RIGHT:
      printf("\033[0;32mXX");
      break;
    case LOCATION_WRONG:
      printf("\033[0;33mXX");
      break;
    case LOCATION_DISCARDED:
      printf("  ");
      break;
    default:
      assert(!"location name not found...");
  };
  printf("\033[0;37m]");
  printf("\033[0m");
}

static struct color locations[] = {
  { 62, 170, 66 },   /* right location (green). */
  { 205, 135, 41 },  /* wrong location (orange). */
  { 58, 58, 60 },    /* discarded location (light grey). */
  { 14, 14, 15 },    /* empty location (dark grey). */
};

static enum location find_location(struct color *color)
{
  for (unsigned i = 0; i < LOCATION_END; ++i) {
    if (color_eq(color, &locations[i]) == true) {
      return i;
    }
  }
  return LOCATION_END;
}

static struct color first_location = { 115, 173, 255 };
static struct color border = { 47, 47, 47 };
static struct color empty = { 14, 14, 15 };

struct bot {
  /* x11 */
  Display *display;
  Window win;
  int screen;
  Colormap map;
  XImage *image;
  /* screen size */
  unsigned width;
  unsigned height;
  /* locations */
  struct coord first; /* up-left location */
  unsigned slot_sz;
  unsigned space_sz;
};

static void image_refresh(struct bot *bot)
{
  if (bot->image != NULL) {
    XFree(bot->image);
  }
  bot->image = XGetImage(bot->display, bot->win, 0, 0,
                         bot->width, bot->height, AllPlanes, XYPixmap);
}

static void color_get(struct bot *bot, int x, int y, struct color *color)
{
  XColor xcolor;

  xcolor.pixel = XGetPixel(bot->image, x, y);
  XQueryColor(bot->display, bot->map, &xcolor);

  color->r = xcolor.red / 256;
  color->g = xcolor.green / 256;
  color->b = xcolor.blue / 256;
}

static bool bot_init(struct bot *bot)
{
  memset(bot, 0, sizeof(*bot));

  bot->display = XOpenDisplay(NULL);
  if (bot->display == NULL) {
    ERR("cannot open display with x11");
    return false;
  }

  bot->screen = DefaultScreen(bot->display);
  bot->win = RootWindow(bot->display, bot->screen);
  bot->map = DefaultColormap(bot->display, bot->screen);
  bot->width = DisplayWidth(bot->display, bot->screen);
  bot->height = DisplayHeight(bot->display, bot->screen);
  printf("[bot] display size: %d x %d\n", bot->width, bot->height);

  return true;
}

static void get_coord(struct bot *bot, unsigned round, unsigned i, struct coord *coord)
{
  coord->x = bot->first.x + i * (bot->slot_sz + bot->space_sz);
  coord->y = bot->first.y + round * (bot->slot_sz + bot->space_sz);
}

static void set_current(struct wordle *wordle, unsigned i, enum location location)
{
  switch (location) {
    case LOCATION_RIGHT:
      wordle->current[i] = RIGHT_LOCATION;
      break;
    case LOCATION_WRONG:
      wordle->current[i] = WRONG_LOCATION;
      break;
    case LOCATION_DISCARDED:
      wordle->current[i] = DISCARDED;
      break;
    default:
      ;
  };
}

static bool get_locations_status(struct bot *bot, struct wordle *wordle, unsigned round)
{
  struct coord coord;
  struct color color;
  unsigned right_location = 0;

  printf("[bot] {round:%u} ", round + 1);
  for (unsigned i = 0; i < WORD_LEN; ++i) {
    get_coord(bot, round, i, &coord);
    color_get(bot, coord.x + 10, coord.y + 10, &color);
    enum location location = find_location(&color);
    assert(location != LOCATION_END);
    if (location == LOCATION_RIGHT) {
      ++right_location;
    }
    set_current(wordle, i, location);
    dump_location_status(location);
  }
  printf("\n");
  return right_location == WORD_LEN;
}

static void press_key(struct bot *bot, KeyCode keycode)
{
#define TIME_50MS 50000
  XTestFakeKeyEvent(bot->display, keycode, true, 0);
  XFlush(bot->display);
  usleep(TIME_50MS);
  XTestFakeKeyEvent(bot->display, keycode, false, 0);
  XFlush(bot->display);
#undef TIME_50MS
}

static void write_word(struct bot *bot, const char *word)
{
  char letter[2];

  for (unsigned i = 0; i < WORD_LEN; ++i) {
    letter[0] = word[i];
    letter[1] = 0;
    KeyCode keycode = XKeysymToKeycode(bot->display, XStringToKeysym(letter));
    press_key(bot, keycode);
  }
  press_key(bot, 36); // return
}


static void set_candidate(struct wordle *wordle, const char *candidate, unsigned round)
{
  BOT("{round:%u} set next candidate: %.*s", round, WORD_LEN, candidate);
  memcpy(wordle->candidate, candidate, WORD_LEN);
}

static struct coord get_mouse_coordinates(struct bot *bot)
{
  struct coord coord;
  int childx, childy;
  unsigned int mask;
  Window w1, w2;

  XQueryPointer(bot->display, bot->win, &w1, &w2,
        	&coord.x, &coord.y, &childx, &childy, &mask);
  BOT("relative coordinates of the root window: (x:%u,y:%u)", coord.x, coord.y);
  return coord;
}

static bool find_first_location(struct bot *bot, struct coord *start_pos)
{
  struct color color;

  for (unsigned y = start_pos->y; y < bot->height; ++y) {
    for (unsigned x = start_pos->x; x < bot->width; ++x) {
      color_get(bot, x, y, &color);
      if (color_eq(&color, &first_location) == true) {
        BOT("first location coordinates: (x:%u,y:%u)", x, y);
        coord_set(&bot->first, x, y);
        return true;
      }
    }
  }
  ERR("cannot find the first location");
  return false;
}

/* Set the number of the pixels for a location. */
static bool set_location_size(struct bot *bot)
{
  unsigned sz = 0;
  struct color color = { 0 };

  for (unsigned x = bot->first.x; x < bot->width; ++x) {
    color_get(bot, x, bot->first.y, &color);
    if (color_eq(&color, &empty) == true) {
      BOT("size of a location: %u", sz);
      bot->slot_sz = sz;
      return true;
    }
    ++sz;
  }
  ERR("cannot set location size");
  return false;
}

/* Set the number of pixels between two locations. */
static bool set_location_space(struct bot *bot)
{
  unsigned sz = 0;
  struct color color = { 0 };

  for (unsigned x = bot->first.x + bot->slot_sz + 1; x < bot->width; ++x) {
    color_get(bot, x, bot->first.y, &color);
    if (color_eq(&color, &border) == true) {
      BOT("space between two locations: %u", sz);
      bot->space_sz = sz;
      return true;
    }
    ++sz;
  }
  return false;
}

static void set_focus(void)
{
#define WAITING_TIME 3u /* 3 seconds */
  BOT("set the focus (by clicking) on the wordle tabulation...");
  BOT("and be the closest to the top left corner of the first location...");
  for (unsigned i = 0; i < WAITING_TIME; ++i) {
    BOT("%u/%u...", i + 1, WAITING_TIME);
    sleep(1);
  }
#undef WAITING_TIME
}

static bool location_init(struct bot *bot)
{
  set_focus();
  struct coord start_pos = get_mouse_coordinates(bot);
  image_refresh(bot);
  if (find_first_location(bot, &start_pos) == false) {
    return false;
  }
  if (set_location_size(bot) == false) {
    return false;
  }
  if (set_location_space(bot) == false) {
    return false;
  }
  return true;
}

static void wait_round_end(struct bot *bot, unsigned round)
{
  struct coord coord;
  struct color color = empty;

#define TIME_300MS 300000
  while (color_eq(&color, &locations[LOCATION_RIGHT]) == false &&
         color_eq(&color, &locations[LOCATION_WRONG]) == false &&
         color_eq(&color, &locations[LOCATION_DISCARDED]) == false) {
    image_refresh(bot);
    get_coord(bot, round, 4, &coord); /* last location */
    color_get(bot, coord.x + 10, coord.y + 10, &color);
    usleep(TIME_300MS);
  }
#undef TIME_300MS
}

int main(void)
{
  const char *next_candidate = FIRST_CANDIDATE;
  struct bot bot;
  struct wordle wordle;

  if (wordle_init(&wordle, words_5, nr_word_5) == false) {
    return -1;
  }
  if (bot_init(&bot) == false) {
    return -1;
  }
  if (location_init(&bot) == false) {
    return -1;
  }

  for (unsigned round = 0; round < NR_MAX_ROUND; ++round) {
    set_candidate(&wordle, next_candidate, round + 1);
    write_word(&bot, wordle.candidate);
    wait_round_end(&bot, round);
    if (get_locations_status(&bot, &wordle, round) == true) {
      BOT(" <<<<<< WIN >>>>>>>>");
      break;
    }
    update_status(&wordle);
    dump_status(&wordle, round + 1, "bot");
    next_candidate = find_next_candidate(&wordle);
    assert(next_candidate != NULL);
  }

  return 0;
}
