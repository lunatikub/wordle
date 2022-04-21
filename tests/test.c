#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "test.h"

static inline uint64_t
get_ns(void)
{
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    abort();
  }
  return ts.tv_sec * UINT64_C(1000000000) + ts.tv_nsec;
}

static bool
do_test(const struct test* t)
{
  bool ok = true;
  printf(" -\n");
  printf(YELLOW " [start test] " PURPLE "(%s)" RESET BOLD " %s\n" RESET,
         t->section,
         t->name);
  const uint64_t start = get_ns();
  ok = t->test_f();
  const uint64_t end = (get_ns() - start) / 1000;
  const char* res = ok ? GREEN "OK" RESET : RED "KO" RESET;
  printf(YELLOW " [end test] %s (%lums)\n", res, end);
  return ok;
}

bool
run_test(const struct test_suite* ts, const char* section, const char* name)
{
  bool ok = true;
  const struct test* t;
  printf(CYAN "[start test] " BOLD "%s\n" RESET, section);
  const uint64_t start = get_ns();
  for (t = &ts->tests[0]; t != &ts->tests[ts->nr_test]; ++t) {
    if (strcmp(t->section, section) == 0 && strcmp(t->name, name) == 0) {
      if (do_test(t) == false) {
        ok = false;
        break;
      }
    }
  }
  const uint64_t end = (get_ns() - start) / 1000;
  const char* res = ok ? GREEN "OK" RESET : RED "KO" RESET;
  printf(CYAN "[end test] %s (%lums)\n", res, end);
  return ok;
}

bool
run_test_section(const struct test_suite* ts, const char* section)
{
  bool ok = true;
  const struct test* t;
  printf(CYAN "[start test section] " BOLD "%s\n" RESET, section);
  const uint64_t start = get_ns();
  for (t = &ts->tests[0]; t != &ts->tests[ts->nr_test]; ++t) {
    if (strcmp(t->section, section) == 0) {
      if (do_test(t) == false) {
        ok = false;
        break;
      }
    }
  }
  const uint64_t end = (get_ns() - start) / 1000;
  const char* res = ok ? GREEN "OK" RESET : RED "KO" RESET;
  printf(CYAN "[end test section] %s (%lums)\n", res, end);
  return ok;
}

bool
run_test_suite(const struct test_suite* ts)
{
  bool ok = true;
  const struct test* t;
  printf(CYAN "[start test suite] " BOLD "%s\n" RESET, ts->name);
  const uint64_t start = get_ns();
  for (t = &ts->tests[0]; t != &ts->tests[ts->nr_test]; ++t) {
    if (do_test(t) == false) {
      ok = false;
      break;
    }
  }
  const uint64_t end = (get_ns() - start) / 1000;
  const char* res = ok ? GREEN "OK" RESET : RED "KO" RESET;
  printf(CYAN "[end test suite] %s (%lums)\n", res, end);
  return ok;
}
