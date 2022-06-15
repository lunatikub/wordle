#include <limits.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "utils_x11.h"
#include "wordle.h"
#include "color.h"
#include "options.h"

/**
 * This bot plays on the following URL: https://wordleplay.com/fr/octordle
 */

/* Maximum number of round. */
#define NR_ROUND 13

/* Number of grid. */
#define NR_GRID 8

/**
 * This is the margin between the left-up corner
 * of a location and the color inside the location.
 * ___
 * | |
 * ---
 * This is in pixels.
 */
#define MARGIN 5

static const struct color c_empty = { 231, 235, 241 };
static const struct color c_white = { 255, 255, 255 };
static const struct color c_right = { 87, 172, 120 };
static const struct color c_wrong = { 233, 198, 1 };
static const struct color c_discarded = { 162, 162, 162 };

struct octordle {
  struct wordle wordle[NR_GRID];
  struct utils_x11 x11;
  const struct word *words;
  struct coord first_loc;
  unsigned width_sz;
  unsigned height_sz;
  unsigned space_sz;
  unsigned space_grid_sz;
  bool solved[NR_GRID];
  unsigned nr_solved;
};

static void octordle_get_location(struct octordle *octordle, unsigned grid,
                                  unsigned round, unsigned i, struct coord *coord)
{
  coord->x =
    octordle->first_loc.x +
    octordle->space_grid_sz * grid +
    i * (octordle->space_sz + octordle->width_sz);

  coord->y = octordle->first_loc.y +
    round * (octordle->height_sz + octordle->space_sz);
}

static bool octordle_set_locations(struct octordle *octordle)
{
  struct coord start;
  struct coord from;
  struct coord to;

  printf("[octordle] set the focus (by clicking) on the tabulation...\n");
  printf("[octordle] and be closest to midle of the first location of the second line...\n");
  utils_x11_focus(&octordle->x11, &start, 3, "octordle");

  /* get width of a location */
  from = start;
  assert(utils_x11_find_h_inc_from(&octordle->x11, &from, &to, &c_empty));
  from = to;
  assert(utils_x11_find_h_inc_from(&octordle->x11, &from, &to, &c_white));
  octordle->width_sz = to.x - from.x - 1;
  printf("[octordle] width location size: %u\n", octordle->width_sz);

  /* get height of a location */
  from.x = from.x + octordle->width_sz / 2;
  assert(utils_x11_find_v_dec_from(&octordle->x11, &from, &to, &c_white));
  from = to;
  ++from.y;
  assert(utils_x11_find_v_inc_from(&octordle->x11, &from, &to, &c_white));
  octordle->height_sz = to.y - from.y;
  printf("[octordle] height location size: %u\n", octordle->height_sz);

  /* get space between two locations */
  --from.y;
  assert(utils_x11_find_v_dec_from(&octordle->x11, &from, &to, &c_empty));
  octordle->space_sz = from.y - to.y + 1;
  printf("[octordle] space size between two locations: %u\n", octordle->space_sz);

  /* find the y coordinate of the left up location */
  from = to;
  assert(utils_x11_find_v_dec_from(&octordle->x11, &from, &to, &c_white));
  octordle->first_loc.y = to.y - 1;

  /* find the x coordinate of the left up location */
  from.y = from.y + octordle->height_sz / 2;
  assert(utils_x11_find_h_dec_from(&octordle->x11, &from, &to, &c_white));
  octordle->first_loc.x = to.x + 1;

  from = octordle->first_loc;
  octordle->first_loc.x += MARGIN;
  octordle->first_loc.y += MARGIN;
  printf("[octordle] first location: (%u,%u)\n", octordle->first_loc.x, octordle->first_loc.y);

  /* find the space between two grids */
  from.x += octordle->width_sz * octordle->words->len + octordle->space_sz * octordle->words->len;
  from.y += 10;
  assert(utils_x11_find_h_inc_from(&octordle->x11, &from, &to, &c_white));
  from = to;
  assert(utils_x11_find_h_inc_from(&octordle->x11, &from, &to, &c_empty));
  octordle->space_grid_sz = (to.x - from.x) +
    octordle->words->len * octordle->width_sz + octordle->words->len * octordle->space_sz;
  printf("[octordle] space between two grids: %u\n", octordle->space_grid_sz);

  return true;
}

static bool octordle_init(struct octordle *octordle, struct options *opts)
{
  memset(octordle, 0, sizeof(*octordle));

  if (utils_x11_init(&octordle->x11) == false) {
    return false;
  }
  printf("[octordle] screen size: %d x %d\n", octordle->x11.width, octordle->x11.height);

  octordle->words = words_find("wordleplay", opts->lang, opts->len);
  assert(octordle->words != NULL);

  for (unsigned i = 0; i < NR_GRID; ++i) {
    if (wordle_init(&octordle->wordle[i], octordle->words) == false) {
      return false;
    }
  }
  return true;
}

static void octordle_set_candidate(struct octordle *octordle, const char *candidate)
{

  for (unsigned i = 0; i < NR_GRID; ++i) {
    memcpy(octordle->wordle[i].candidate, candidate, octordle->words->len);
  }
  printf("[octordle] set next candidate: %s\n", candidate);
}

static void octordle_wait_round_end(struct octordle *octordle, unsigned round)
{
  struct coord coord;
  struct color color = { 0, 0, 0 };

#define WAITING_TIME 20000 /* ms */
  while (color_approx_eq(&color, &c_right) == false &&
         color_approx_eq(&color, &c_wrong) == false &&
         color_approx_eq(&color, &c_discarded) == false) {
    utils_x11_image_refresh(&octordle->x11);
    /* last location of the last grid of the line `round` */
    octordle_get_location(octordle, NR_GRID - 1, round, octordle->words->len - 1, &coord);
    utils_x11_color_get(&octordle->x11, coord.x, coord.y, &color);
    usleep(WAITING_TIME);
  }
#undef WAITING_TIME
}

static bool octordle_get_locations_status(struct octordle *o, unsigned round)
{
  struct coord coord;
  struct color color;
  enum status status;

  printf("[octordle] {round:%u} ", round + 1);
  for (unsigned grid = 0; grid < NR_GRID; ++grid) {
    unsigned right_location = 0;
    printf("[");
    for (unsigned i = 0; i < o->words->len; ++i) {
      octordle_get_location(o, grid, round, i, &coord);
      utils_x11_color_get(&o->x11, coord.x, coord.y, &color);
      status = status_map_from_colors(&color, &c_right, &c_wrong, &c_discarded);
      assert(status != UNKNOWN);
      right_location += status == RIGHT ? 1 : 0;
      o->wordle[grid].current[i] = status;
      status_dump(status);
      if (right_location == o->words->len) {
        o->solved[grid] = true;
        ++o->nr_solved;
      }
    }
    printf("] ");
  }
  printf("\n");
  if (o->nr_solved == NR_GRID) {
    printf("[octordle] <<<<<< WIN >>>>>>\n");
    return true;
  }
  printf("[octordle] number of solved grid: %u\n", o->nr_solved);
  return false;
}

static const char* octordle_find_next_candidate(struct octordle *o, unsigned round)
{
  unsigned grid;
  unsigned best_grid = 0;
  unsigned best_nr_candidate = UINT_MAX;
  unsigned nr_candidate = 0;

  /* select the first non-solved grid */
  for (grid = 0; grid < NR_GRID; ++grid) {
    if (o->solved[grid] == false) {
      best_grid = grid;
      break;
    }
  }
  printf("[octordle] first grid unsolved: %u\n", best_grid);

  printf("[octordle] {round:%u} ", round + 1);
  for (grid = 0; grid < NR_GRID; ++grid) {
    if (o->solved[grid] == true) {
      continue;
    }
    struct wordle *w = &o->wordle[grid];
    wordle_update_status(w);
    nr_candidate = wordle_count_number_of_candidates(w);
    printf("grid(%u):%u, ", grid, nr_candidate);
    if (nr_candidate < best_nr_candidate) {
      best_nr_candidate = nr_candidate;
      best_grid = grid;
    }
  }
  printf("\n");

  const char *next_candidate = wordle_find_next_candidate(&o->wordle[best_grid]);
  assert(next_candidate != NULL);
  printf("[octordle] {round:%u} grid %u, nr:%u, candidate: %s\n",
      round + 1, best_grid, best_nr_candidate, next_candidate);
  return next_candidate;
}

int main(int argc, char **argv)
{
  struct octordle octordle;
  struct options opts;

  options_parse(argc, argv, &opts);
  octordle_init(&octordle, &opts);

  if (octordle_set_locations(&octordle) == false) {
    return -1;
  }

  const char *next_candidate = opts.first == NULL ?
    words_find_best_candidate(octordle.words) : opts.first;

  for (unsigned round = 0; round < NR_ROUND; ++round) {
    octordle_set_candidate(&octordle, next_candidate);
    utils_x11_write(&octordle.x11, next_candidate, octordle.words->len);
    octordle_wait_round_end(&octordle, round);
    octordle_get_locations_status(&octordle, round);
    next_candidate = octordle_find_next_candidate(&octordle, round);
  }

  return 0;
}
