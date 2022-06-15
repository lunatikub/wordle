#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "nerdle/nerdle.h"
#include "nerdle/equation.h"

#include "utils_x11.h"
#include "options.h"
#include "status.h"

/**
 * This bot plays on the following URL: https://wordleplay.com/fr/nerdle
 */

/* Maximum number of round. */
#define NR_ROUND 6

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

struct handle {
  struct nerdle *nerdle;
  struct utils_x11 x11;
  struct coord first_loc;
  unsigned width_sz;
  unsigned height_sz;
  unsigned space_sz;
};

static void nerdle_get_location(struct handle *handle, unsigned round,
                                unsigned i, struct coord *coord)
{
  coord->x = handle->first_loc.x + (handle->space_sz + handle->width_sz) * i;
  coord->y = handle->first_loc.y + (handle->space_sz + handle->height_sz) * round;
}

static bool nerdle_init(struct handle *handle, struct options *opts)
{
  memset(handle, 0, sizeof(*handle));
  handle->nerdle = nerdle_create(opts->len);

  if (utils_x11_init(&handle->x11) == false) {
    return false;
  }
  printf("[nerdle] screen size: %d x %d\n", handle->x11.width, handle->x11.height);

  return true;
}

static bool nerdle_set_locations(struct handle *handle)
{
  struct coord start;
  struct coord from;
  struct coord to;

  printf("[nerdle] set the focus (by clicking) on the tabulation...\n");
  printf("[nerdle] and be closest to midle of the first location of the second line...\n");
  printf("[nerdle]            ---   ---     \n");
  printf("[nerdle]  here --> |   | |   |    \n");
  printf("[nerdle]            ---   ---     \n");
  printf("[nerdle]            ---   ---     \n");
  printf("[nerdle]           |   | |   |    \n");
  printf("[nerdle]            ---   ---  ...\n");
  printf("[nerdle]           .\n");
  printf("[nerdle]           .\n");
  printf("[nerdle]           .\n");
  utils_x11_focus(&handle->x11, &start, 3, "nerdle");

  /* get the width of a location */
  from = start;
  assert(utils_x11_find_h_inc_from(&handle->x11, &from, &to, &c_empty));
  from = to;
  assert(utils_x11_find_h_inc_from(&handle->x11, &from, &to, &c_white));
  handle->width_sz = to.x - from.x - 1;
  printf("[handle] width location size: %u\n", handle->width_sz);

  /* get height of a location */
  from.x = from.x + handle->width_sz / 2;
  assert(utils_x11_find_v_dec_from(&handle->x11, &from, &to, &c_white));
  from = to;
  ++from.y;
  assert(utils_x11_find_v_inc_from(&handle->x11, &from, &to, &c_white));
  handle->height_sz = to.y - from.y;
  printf("[handle] height location size: %u\n", handle->height_sz);

  /* get space between two locations */
  from = to;
  ++to.y;
  assert(utils_x11_find_v_inc_from(&handle->x11, &from, &to, &c_empty));
  handle->space_sz = to.y - from.y;
  printf("[handle] space size between two locations: %u\n", handle->space_sz);

  /* find the y coordinate of the left up location */
  from.y--;
  assert(utils_x11_find_v_dec_from(&handle->x11, &from, &to, &c_white));
  handle->first_loc.y = to.y - 1;

  /* find the x coordinate of the left up location */
  from.y = from.y + handle->height_sz / 2;
  assert(utils_x11_find_h_dec_from(&handle->x11, &from, &to, &c_white));
  handle->first_loc.x = to.x + 1;

  handle->first_loc.x += MARGIN;
  handle->first_loc.y += MARGIN;
  printf("[nerdle] first location: (%u,%u)\n", handle->first_loc.x, handle->first_loc.y);

  return true;
}

static void nerdle_wait_round_end(struct handle *handle, unsigned round)
{
  struct coord coord;
  struct color color = { 0, 0, 0 };

#define WAITING_TIME 20000 /* ms */
  while (color_approx_eq(&color, &c_right) == false &&
         color_approx_eq(&color, &c_wrong) == false &&
         color_approx_eq(&color, &c_discarded) == false) {
    utils_x11_image_refresh(&handle->x11);
    /* last location of the last grid of the line `round` */
    nerdle_get_location(handle, round, handle->nerdle->len - 1, &coord);
    utils_x11_color_get(&handle->x11, coord.x, coord.y, &color);
    usleep(WAITING_TIME);
  }
#undef WAITING_TIME
}

static void update_status(struct nerdle *nerdle, enum status status,
                          const char *equation, unsigned i)
{
  unsigned mapped = nerdle_map_alpha(equation[i]);
  switch (status) {
    case DISCARDED:
      nerdle->discarded[mapped] = true;
      break;
    case WRONG:
      nerdle->wrong[mapped][i] = true;
      break;
    case RIGHT:
      nerdle->right[i] = equation[i];
      break;
    default:
      printf("WTF\n");
  };
}

static bool nerdle_get_locations_status(struct handle *handle, unsigned round,
                                        const char *equation)
{
  struct coord coord;
  struct color color;
  enum status status;
  unsigned right_location = 0;

  printf("[nerdle] {round:%u} ", round + 1);

  printf("[");
  for (unsigned i = 0; i < handle->nerdle->len; ++i) {
    nerdle_get_location(handle, round, i, &coord);
    utils_x11_color_get(&handle->x11, coord.x, coord.y, &color);
    status = status_map_from_colors(&color, &c_right, &c_wrong, &c_discarded);
    assert(status != UNKNOWN);
    right_location += status == RIGHT ? 1 : 0;
    status_dump(status);
    update_status(handle->nerdle, status, equation, i);
  }
  printf("] ");
  printf("\n");
  if (right_location == handle->nerdle->len) {
    printf("[nerdle] <<<<<< WIN >>>>>>\n");
    return true;
  }
  return false;
}

int main(int argc, char **argv)
{
  struct options opts;
  struct handle handle;

  options_parse(argc, argv, &opts);

  if (nerdle_init(&handle, &opts) == false) {
    return -1;
  }
  if (nerdle_set_locations(&handle) == false) {
    return -1;
  }

  const char *equation = nerdle_first_equation(handle.nerdle);

  for (unsigned round = 0; round < NR_ROUND; ++round) {
    printf("[nerdle] best equation: %s\n", equation);
    utils_x11_write(&handle.x11, equation, handle.nerdle->len);
    nerdle_wait_round_end(&handle, round);
    if (nerdle_get_locations_status(&handle, round, equation) == true) {
      break;
    }
    if (round == 0) {
        nerdle_generate_equations(handle.nerdle);
    }
    nerdle_select_equations(handle.nerdle);
    equation = nerdle_find_best_equation(handle.nerdle);
  }

  nerdle_destroy(handle.nerdle);
  return 0;
}
