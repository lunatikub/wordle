#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "nerdle/nerdle.h"

static inline uint64_t get_time_ms(void)
{
  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    perror("clock_gettime");
    abort();
  }
  return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

int main(void)
{
  for (unsigned len = 5; len < 12; ++len) {
    struct nerdle *nerdle = nerdle_create(len);
    uint64_t start = get_time_ms();
    nerdle_generate_equations(nerdle);
    uint64_t time = get_time_ms() - start;
    printf("len: %u, nr_candidate: %u, nr_fail: %u, nr_call: %u,"
           " time: %lu(us), %lu(ms), %lu(s)\n",
           len, nerdle->nr_candidate, nerdle->nr_fail, nerdle->nr_call,
           time, time / 1000, time / 1000000);
    nerdle_destroy(nerdle);
  }

  return 0;
}
