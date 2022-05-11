#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "bot.h"

#include "fr_words_5.h"
#include "fr_words_6.h"
#include "fr_words_7.h"
#include "fr_words_8.h"
#include "fr_words_9.h"

static bool bot_words_mapping(struct bot *bot, unsigned len)
{
  bot->len = len;

  switch (len) {
    case 5:
      bot->nr = nr_word_5;
      bot->words = words_5;
      break;
    case 6:
      bot->nr = nr_word_6;
      bot->words = words_6;
      break;
    case 7:
      bot->nr = nr_word_7;
      bot->words = words_7;
      break;
    case 8:
      bot->nr = nr_word_8;
      bot->words = words_8;
      break;
    case 9:
      bot->nr = nr_word_9;
      bot->words = words_9;
      break;
    default:
      return false;
  };
  return true;
}

bool bot_init(struct bot *bot, unsigned len, struct color *borders, struct color *locations)
{
  memset(bot, 0, sizeof(*bot));

  if (utils_x11_init(&bot->x11) == false) {
    ERR("cannot init x11");
    return false;
  }
  if (bot_words_mapping(bot, len) == false) {
    return false;
  }

  memcpy(bot->borders, borders, sizeof(struct color) * BORDER_END);
  memcpy(bot->locations, locations, sizeof(struct color) * LOCATION_END);

  BOT("screen size: %d x %d", bot->x11.width, bot->x11.height);

  if (wordle_init(&bot->wordle, bot->words, bot->len, bot->nr) == false) {
    return -1;
  }

  return true;
}

void bot_focus(struct bot *bot)
{
#define WAITING_TIME 3u /* 3 seconds */
  BOT("set the focus (by clicking) on the wordle tabulation...");
  BOT("and be closest to the top left corner of the first location...");
  for (unsigned i = 0; i < WAITING_TIME; ++i) {
    BOT("%u/%u...", i + 1, WAITING_TIME);
    sleep(1);
  }
#undef WAITING_TIME

  bot->start = utils_x11_get_mouse_coordinates(&bot->x11);
  utils_x11_image_refresh(&bot->x11);
}

void bot_get_loc_coord(struct bot *bot, unsigned round, unsigned i, struct coord *coord)
{
  coord->x = bot->first.x + i * bot->width_loc;
  coord->y = bot->first.y + round * bot->height_loc;
}

void bot_wait_round_end(struct bot *bot, unsigned round)
{
  struct coord coord;
  struct color color = { 0, 0, 0 };

#define TIME_300MS 300000
  while (color_approx_eq(&color, &bot->locations[LOCATION_RIGHT]) == false &&
         color_approx_eq(&color, &bot->locations[LOCATION_WRONG]) == false &&
         color_approx_eq(&color, &bot->locations[LOCATION_DISCARDED]) == false) {
    utils_x11_image_refresh(&bot->x11);
    bot_get_loc_coord(bot, round, bot->len - 1, &coord); /* last location of the line. */
    utils_x11_color_get(&bot->x11, coord.x, coord.y, &color);
    usleep(TIME_300MS);
  }
#undef TIME_300MS
}

/**
 * Dump in color the status of the locations at the end of a round.
 */
static void bot_dump_location_status(enum location location)
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

/**
 * Set the status for a location.
 */
static void bot_set_location(struct bot *bot, unsigned i, enum location location)
{
  switch (location) {
    case LOCATION_RIGHT:
      bot->wordle.current[i] = RIGHT_LOCATION;
      break;
    case LOCATION_WRONG:
      bot->wordle.current[i] = WRONG_LOCATION;
      break;
    case LOCATION_DISCARDED:
      bot->wordle.current[i] = DISCARDED;
      break;
    default:
      ;
  };
}

static enum location find_location(struct bot *bot, struct color *color)
{
  for (unsigned i = 0; i < LOCATION_END; ++i) {
    if (color_approx_eq(color, &bot->locations[i]) == true) {
      return i;
    }
  }
  return LOCATION_END;
}

bool bot_get_locations_status(struct bot *bot, unsigned round)
{
  struct coord coord;
  struct color color;
  unsigned right_location = 0;

  printf("[bot] {round:%u} ", round + 1);
  for (unsigned i = 0; i < bot->wordle.len; ++i) {
    bot_get_loc_coord(bot, round, i, &coord);
    utils_x11_color_get(&bot->x11, coord.x, coord.y, &color);
    enum location location = find_location(bot, &color);
    assert(location != LOCATION_END);
    if (location == LOCATION_RIGHT) {
      ++right_location;
    }
    bot_set_location(bot, i, location);
    bot_dump_location_status(location);
  }
  printf("\n");
  if (right_location == bot->len) {
    BOT(" <<<<<< WIN >>>>>>>>");
    return true;
  }
  return false;
}

void bot_set_candidate(struct bot *bot, const char *candidate)
{
  memcpy(bot->wordle.candidate, candidate, bot->len);
  BOT("set next candidate: %s", candidate);
}

const char* bot_find_next_candidate(struct bot *bot, unsigned round)
{
  const char *next_candidate = NULL;
  update_status(&bot->wordle);
  dump_status(&bot->wordle, round + 1, "bot");
  next_candidate = find_next_candidate(&bot->wordle);
  assert(next_candidate != NULL);
  return next_candidate;
}
