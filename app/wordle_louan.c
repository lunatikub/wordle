#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "utils_x11.h"
#include "wordle.h"
#include "color.h"
#include "options.h"

/**
 * This bot plays on the following URL: https://wordle.louan.me
 */

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

struct louan {
  struct wordle wordle;
  struct utils_x11 x11;
  const struct word *words;
  struct coord first_loc;
  unsigned width_sz;
  unsigned height_sz;
};

static void louan_get_location(struct louan *louan, unsigned round,
                               unsigned i, struct coord *coord)
{
  coord->x = louan->first_loc.x + i * louan->width_sz;
  coord->y = louan->first_loc.y + round * louan->height_sz;
}

static bool louan_set_locations(struct louan *louan)
{
  struct coord start_loc;
  struct coord coord;

  printf("[louan] set the focus (by clicking) on the tabulation...\n");
  printf("[louan] and be closest to the top left corner of the first location...\n");
  utils_x11_focus(&louan->x11, &start_loc, 3);

  if (utils_x11_find_from(&louan->x11, &start_loc, &louan->first_loc, &c_first_loc) == false) {
    CANNOT_FIND_COLOR(c_first_loc);
    return false;
  }

  if (utils_x11_find_h_inc_from(&louan->x11, &louan->first_loc, &coord, &c_loc) == false) {
    CANNOT_FIND_COLOR(c_loc);
    return false;
  }
  louan->width_sz = coord.x - louan->first_loc.x;

  if (utils_x11_find_v_inc_from(&louan->x11, &louan->first_loc, &coord, &c_loc) == false) {
    CANNOT_FIND_COLOR(c_loc);
    return false;
  }
  louan->height_sz = coord.y - louan->first_loc.y;

  louan->first_loc.x += MARGIN;
  louan->first_loc.y += MARGIN;

  printf("[louan] first location coordinates: (x:%u,y:%u)\n", louan->first_loc.x, louan->first_loc.y);
  printf("[louan] x location size: %u\n", louan->width_sz);
  printf("[louan] y location size: %u\n", louan->height_sz);

  return true;
}

static bool louan_init(struct louan *louan)
{
  memset(louan, 0, sizeof(*louan));

  if (utils_x11_init(&louan->x11) == false) {
    return false;
  }

  louan->words = words_find("wordle_louan", FR, 5);
  assert(louan->words != NULL);

  if (wordle_init(&louan->wordle, louan->words) == false) {
    return false;
  }
  return true;
}

static void louan_wait_round_end(struct louan *louan, unsigned round)
{
  struct coord coord;
  struct color color = { 0, 0, 0 };

#define TIME_300MS 300000
  while (color_approx_eq(&color, &c_right) == false &&
         color_approx_eq(&color, &c_wrong) == false &&
         color_approx_eq(&color, &c_discarded) == false) {
    utils_x11_image_refresh(&louan->x11);
    louan_get_location(louan, round, louan->words->len - 1, &coord);
    utils_x11_color_get(&louan->x11, coord.x, coord.y, &color);
    usleep(TIME_300MS);
  }
#undef TIME_300MS
}

static bool louan_get_locations_status(struct louan *l, unsigned round)
{
  struct coord coord;
  struct color color;
  unsigned right_location = 0;

  printf("[louan] {round:%u} ", round + 1);
  for (unsigned i = 0; i < l->words->len; ++i) {
    louan_get_location(l, round, i, &coord);
    utils_x11_color_get(&l->x11, coord.x, coord.y, &color);
    enum status status = wordle_map_from_color(&color, &c_right, &c_wrong, &c_discarded);
    assert(status != UNKNOWN);
    if (status == RIGHT) {
      ++right_location;
    }
    l->wordle.current[i] = status;
    wordle_dump_location_status(status);
  }
  printf("\n");
  if (right_location == l->words->len) {
    printf("[louan] <<<<<< WIN >>>>>>\n");
    return true;
  }
  return false;
}

static const char* louan_find_next_candidate(struct louan *l, unsigned round)
{
  const char *next_candidate = NULL;
  wordle_update_status(&l->wordle);
  wordle_dump_status(&l->wordle, round + 1);
  next_candidate = wordle_find_next_candidate(&l->wordle);
  assert(next_candidate != NULL);
  return next_candidate;
}


int main(int argc, char **argv)
{
  struct louan louan;
  struct options opts;

  options_parse(argc, argv, &opts);

  louan_init(&louan);
  const char *next_candidate = opts.first == NULL ?
    words_find_best_candidate(louan.words) : opts.first;

  if (louan_set_locations(&louan) == false) {
    return -1;
  }

  for (unsigned round = 0; round < NR_ROUND; ++round) {
    wordle_set_candidate(&louan.wordle, next_candidate);
    utils_x11_write(&louan.x11, louan.wordle.candidate, louan.words->len);
    louan_wait_round_end(&louan, round);
    if (louan_get_locations_status(&louan, round) == true) {
      break; /* win */
    }
    next_candidate = louan_find_next_candidate(&louan, round);
  }

  return 0;
}
