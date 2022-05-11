#include "bot.h"

/**
 * This bot plays on the following URL: wordle.louan.me
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

static struct color locations[] = {
  { 62, 170, 66 }, /* right location (green). */
  { 205, 135, 41 }, /* wrong location (orange). */
  { 58, 58, 60 }, /* discarded location (light grey). */
  { 14, 14, 15 }, /* empty location (dark grey). */
};

static struct color borders[] = {
  { 115, 173, 255 }, /* first border (blue sky). */
  { 47, 47, 47 }, /* border (grey). */
};

static bool set_location_properties(struct bot *bot)
{
  struct coord coord;

  bot_focus(bot);

  if (utils_x11_find_color_from(&bot->x11, &bot->start,
                                &bot->first, &bot->borders[BORDER_FIRST]) == false) {
    CANNOT_FIND_COLOR(bot->borders[BORDER_FIRST]);
    return false;
  }

  if (utils_x11_find_horizontal_color_from(&bot->x11, &bot->first,
                                           &coord, &bot->borders[BORDER_LOCATION]) == false) {
    CANNOT_FIND_COLOR(bot->borders[BORDER_LOCATION]);
    return false;
  }
  bot->width_loc = coord.x - bot->first.x;

  if (utils_x11_find_vertical_color_from(&bot->x11, &bot->first,
                                         &coord, &bot->borders[BORDER_LOCATION]) == false) {
    CANNOT_FIND_COLOR(bot->borders[BORDER_LOCATION]);
    return false;
  }
  bot->height_loc = coord.y - bot->first.y;

  bot->first.x += MARGIN;
  bot->first.y += MARGIN;

  BOT("first location coordinates: (x:%u,y:%u)", bot->first.x, bot->first.y);
  BOT("x location size: %u", bot->width_loc);
  BOT("y location size: %u", bot->height_loc);

  return true;
}

int main(void)
{
  const char *next_candidate = FIRST_CANDIDATE;
  struct bot bot;

  if (bot_init(&bot, 5, borders, locations) == false) {
    return -1;
  }
  if (set_location_properties(&bot) == false) {
    return -1;
  }

  for (unsigned round = 0; round < NR_ROUND; ++round) {
    bot_set_candidate(&bot, next_candidate);
    utils_x11_write(&bot.x11, bot.wordle.candidate, bot.len);
    bot_wait_round_end(&bot, round);
    if (bot_get_locations_status(&bot, round) == true) {
      break;
    }
    next_candidate = bot_find_next_candidate(&bot, round);
  }

  return 0;
}
