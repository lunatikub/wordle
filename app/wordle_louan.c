#include <string.h>

#include "fr_words_5.h"
#include "bot.h"

/**
 * This bot plays on the following URL: https://wordle.louan.me
 */

/* One of the best word to start with. */
#define FIRST_CANDIDATE "tarie"

/* Maximum number of round */
#define NR_ROUND 6

/**
 * This is the margin between the left-up corner
 * of a location and the color inside the location.
 * ___
 * | |
 * ---
 * This is in pixels.
 */
#define MARGIN (10)

static const struct color c_right = { 62, 170, 66 };
static const struct color c_wrong = { 205, 135, 41 };
static const struct color c_discarded = { 58, 58, 60 };
static const struct color c_first_loc = { 115, 173, 255 };
static const struct color c_loc = { 47, 47, 47 };

struct wordle_louan {
  struct wordle wordle;
  struct utils_x11 x11;
  struct word words;
  struct coord first_loc;
  unsigned width_sz;
  unsigned height_sz;
};

static void wl_get_location(void *handle, unsigned round,
                            unsigned i, struct coord *coord)
{
  struct wordle_louan *wl = handle;

  coord->x = wl->first_loc.x + i * wl->width_sz;
  coord->y = wl->first_loc.y + round * wl->height_sz;
}

static bool wordle_louan_set_locations(struct wordle_louan *wl)
{
  struct coord start_loc;
  struct coord coord;

  BOT("set the focus (by clicking) on the tabulation...");
  BOT("and be closest to the top left corner of the first location...");
  bot_focus(&wl->x11, &start_loc, 3);

  if (utils_x11_find_from(&wl->x11, &start_loc, &wl->first_loc, &c_first_loc) == false) {
    CANNOT_FIND_COLOR(c_first_loc);
    return false;
  }

  if (utils_x11_find_h_from(&wl->x11, &wl->first_loc, &coord, &c_loc) == false) {
    CANNOT_FIND_COLOR(c_loc);
    return false;
  }
  wl->width_sz = coord.x - wl->first_loc.x;

  if (utils_x11_find_v_from(&wl->x11, &wl->first_loc, &coord, &c_loc) == false) {
    CANNOT_FIND_COLOR(c_loc);
    return false;
  }
  wl->height_sz = coord.y - wl->first_loc.y;

  wl->first_loc.x += MARGIN;
  wl->first_loc.y += MARGIN;

  BOT("first location coordinates: (x:%u,y:%u)", wl->first_loc.x, wl->first_loc.y);
  BOT("x location size: %u", wl->width_sz);
  BOT("y location size: %u", wl->height_sz);

  return true;
}

static bool wordle_louan_init(struct wordle_louan *wl)
{
  memset(wl, 0, sizeof(*wl));

  if (utils_x11_init(&wl->x11) == false) {
    return false;
  }

  wl->words.len = 5;
  wl->words.words = words_5;
  wl->words.nr = nr_word_5;

  if (wordle_init(&wl->wordle, wl->words.words,
                  wl->words.len, wl->words.nr) == false) {
    return false;
  }
  return true;
}

int main(void)
{
  const char *next_candidate = FIRST_CANDIDATE;
  struct wordle_louan wl;

  wordle_louan_init(&wl);

  if (wordle_louan_set_locations(&wl) == false) {
    return -1;
  }

  for (unsigned round = 0; round < NR_ROUND; ++round) {
    bot_set_candidate(&wl.wordle, next_candidate);
    utils_x11_write(&wl.x11, wl.wordle.candidate, wl.words.len);
    bot_wait_round_end(&wl.x11, wl_get_location, &wl, round, wl.words.len - 1,
                       &c_right, &c_wrong, &c_discarded);
    if (bot_get_locations_status(&wl.x11, &wl.wordle,
                                 wl_get_location, &wl, round, wl.words.len,
                                 &c_right, &c_wrong, &c_discarded) == true) {
      break;
    }
    next_candidate = bot_find_next_candidate(&wl.wordle, round);
  }

  return 0;
}
