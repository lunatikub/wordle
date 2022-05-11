#ifndef __BOT__
#define __BOT__

#include <stdbool.h>
#include <stdio.h>

#include "utils_x11.h"
#include "wordle.h"

/* Bot interaction. */
#define BOT(fmt, ...) printf("[bot] " fmt "\n", ##__VA_ARGS__)

/* Error bot interaction. */
#define ERR(fmt, ...) fprintf(stderr, "[err] " fmt "...\n", ##__VA_ARGS__)

/* Cannot find a color on the screen. */
#define CANNOT_FIND_COLOR(COLOR)                                        \
  ERR("cannot find color: (%i, %i, %i)", COLOR.r, COLOR.g, COLOR.b);

enum location {
  LOCATION_RIGHT,
  LOCATION_WRONG,
  LOCATION_DISCARDED,
  LOCATION_EMPTY,
  LOCATION_END,
};

enum border {
  BORDER_FIRST,
  BORDER_LOCATION,
  BORDER_SPACE,
  BORDER_END,
};

struct bot {
  /** Wordle AI. **/
  struct wordle wordle;
  /** All stuffs related with x11. **/
  struct utils_x11 x11;
  /* Locations */
  struct coord start; /* start coordinates to search locations. */
  struct coord first; /* up left location. */
  unsigned width_loc; /* width of a location. */
  unsigned height_loc; /* height of a location. */
  /** Words **/
  unsigned len; /* length of words. */
  unsigned nr; /* number of words. */
  const char **words; /* list of words */
  /** Colors of the game. */
  struct color borders[BORDER_END];
  struct color locations[LOCATION_END];
};

/**
 * Initialize a bot with the color of the games
 * and the length of the words.
 */
bool bot_init(struct bot *bot, unsigned len, struct color *borders, struct color *locations);

/**
 * Asking user to set the focus on the screen of the game to get
 * the start coordinates to search the locations.
 * This part is not mandatory but it optimizes the time to find locations.
 */
void bot_focus(struct bot *bot);

/**
 * Process the coordinates of a location from the round and the letter position.
 * It is based on the fields width_loc and height_loc.
 */
void bot_get_loc_coord(struct bot *bot, unsigned round, unsigned i, struct coord *coord);

/**
 * It is blocking until the last location of the round given in parameter
 * has been updated as a right/wrong/discarded color.
 */
void bot_wait_round_end(struct bot *bot, unsigned round);

/**
 * Get the status of each location at the end of a round.
 */
bool bot_get_locations_status(struct bot *bot, unsigned round);

/**
 * Set the next candidate to evaluate.
 */
void bot_set_candidate(struct bot *bot, const char *candidate);

/**
 * Find the best next candidate.
 */
const char* bot_find_next_candidate(struct bot *bot, unsigned round);

#endif /* !__BOT__ */
