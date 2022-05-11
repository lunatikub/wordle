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

struct word {
  const char **words;
  unsigned nr;
  unsigned len;
};

/**
 * Find the best next candidate.
 */
const char* bot_find_next_candidate(struct wordle *wordle, unsigned round);

/**
 * Asking user to set the focus on the screen of the game to get
 * the start coordinates to search the locations.
 */
void bot_focus(struct utils_x11 *x11, struct coord *coord, unsigned waiting_time);

/**
 * Set the next candidate to evaluate.
 */
void bot_set_candidate(struct wordle *wordle, const char *candidate);

/**
 * Callback to get the coordinates of a location from round and letter position.
 */
typedef void (*get_location_cb)(void *handle, unsigned round, unsigned i, struct coord *coord);

/**
 * It is blocking until the last location of the round given in parameter
 * has been updated as a right/wrong/discarded color.
 */
void bot_wait_round_end(struct utils_x11 *x11,
                        get_location_cb get_location,
                        void *handle,
                        unsigned round, unsigned i,
                        const struct color *c_right,
                        const struct color *c_wrong,
                        const struct color *c_discarded);

/**
 * Get the status of each location at the end of a round.
 */
bool bot_get_locations_status(struct utils_x11 *x11,
                              struct wordle *wordle,
                              get_location_cb get_location,
                              void *handle,
                              unsigned round,
                              unsigned word_len,
                              const struct color *c_right,
                              const struct color *c_wrong,
                              const struct color *c_discarded);

#endif /* !__BOT__ */
