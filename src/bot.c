#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "bot.h"

void bot_wait_round_end(struct utils_x11 *x11,
                        get_location_cb get_location,
                        void *handle,
                        unsigned round,
                        unsigned i,
                        const struct color *c_right,
                        const struct color *c_wrong,
                        const struct color *c_discarded)
{
  struct coord coord;
  struct color color = { 0, 0, 0 };

#define TIME_300MS 300000
  while (color_approx_eq(&color, c_right) == false &&
         color_approx_eq(&color, c_wrong) == false &&
         color_approx_eq(&color, c_discarded) == false) {
    utils_x11_image_refresh(x11);
    get_location(handle, round, i, &coord); /* last location of the line */
    utils_x11_color_get(x11, coord.x, coord.y, &color);
    usleep(TIME_300MS);
  }
#undef TIME_300MS
}

/**
 * Dump in color the status of the locations at the end of a round.
 */
static void bot_dump_location_status(enum status status)
{
  printf("\033[0;37m[");
  switch (status) {
    case RIGHT:
      printf("\033[0;32mXX");
      break;
    case WRONG:
      printf("\033[0;33mXX");
      break;
    case DISCARDED:
      printf("  ");
      break;
    default:
      assert(!"location name not found...");
  };
  printf("\033[0;37m]");
  printf("\033[0m");
}

static enum status get_status_location(struct color *color,
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

bool bot_get_locations_status(struct utils_x11 *x11,
                              struct wordle *wordle,
                              get_location_cb get_location,
                              void *handle,
                              unsigned round,
                              unsigned word_len,
                              const struct color *c_right,
                              const struct color *c_wrong,
                              const struct color *c_discarded)
{
  struct coord coord;
  struct color color;
  unsigned right_location = 0;

  printf("[bot] {round:%u} ", round + 1);
  for (unsigned i = 0; i < word_len; ++i) {
    get_location(handle, round, i, &coord);
    utils_x11_color_get(x11, coord.x, coord.y, &color);
    enum status status = get_status_location(&color, c_right, c_wrong, c_discarded);
    assert(status != UNKNOWN);
    if (status == RIGHT) {
      ++right_location;
    }
    wordle->current[i] = status;
    bot_dump_location_status(status);
  }
  printf("\n");
  if (right_location == word_len) {
    BOT(" <<<<<< WIN >>>>>>>>");
    return true;
  }
  return false;
}

const char* bot_find_next_candidate(struct wordle *wordle, unsigned round)
{
  const char *next_candidate = NULL;
  update_status(wordle);
  dump_status(wordle, round + 1, "bot");
  next_candidate = find_next_candidate(wordle);
  assert(next_candidate != NULL);
  return next_candidate;
}

void bot_focus(struct utils_x11 *x11, struct coord *coord, unsigned waiting_time)
{
  for (unsigned i = 0; i < waiting_time; ++i) {
    BOT("%u/%u...", i + 1, waiting_time);
    sleep(1);
  }
  *coord = utils_x11_get_mouse_coordinates(x11);
  utils_x11_image_refresh(x11);
}

void bot_set_candidate(struct wordle *wordle, const char *candidate)
{
  memcpy(wordle->candidate, candidate, wordle->len);
  BOT("set next candidate: %s", candidate);
}
