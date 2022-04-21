#ifndef __TEST_H__
#define __TEST_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Colors */
#define RESET "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define YELLOW "\033[0;33m"

#define BOLD "\e[1m"

/* Declare a test. */
#define TEST_F(section, name) static bool test_##section##_##name(void)

/* clang-format off */
/* Add a test to the test suite. */
#define TEST(section, name)                                  \
  {                                                          \
    #section, #name, test_ ## section ## _ ## name           \
  }
/* clang-format on */

/**
 * Structure used to declare a test.
 * Should use helper TEST.
 */
struct test
{
  char* section;
  char* name;
  bool (*test_f)(void);
};

/**
 * Structure used to declare a test suite.
 * Should use helper TEST_SUITE.
 */
struct test_suite
{
  char* name;
  uint32_t nr_test;
  const struct test* tests;
};

/* Run all tests of a test suite. */
bool
run_test_suite(const struct test_suite* ts);

/* Run all tests of a section. */
bool
run_test_section(const struct test_suite* ts, const char* section);

/* Run a specific test. */
bool
run_test(const struct test_suite* ts, const char* section, const char* name);

/* Declare a test suite. */
#define TEST_SUITE(name)                                                       \
  const static struct test_suite name##_test_suite = {                         \
    #name,                                                                     \
    sizeof(name##_tests) / sizeof(struct test),                                \
    name##_tests,                                                              \
  };                                                                           \
  int main(int argc, char** argv)                                              \
  {                                                                            \
    if (argc == 2) {                                                           \
      return run_test_section(&name##_test_suite, argv[1]) ? 0 : -1;           \
    }                                                                          \
    if (argc == 3) {                                                           \
      return run_test(&name##_test_suite, argv[1], argv[2]) ? 0 : -1;          \
    }                                                                          \
    return run_test_suite(&name##_test_suite) ? 0 : -1;                        \
  }

/********************************************************
 * Following macro are helpers to be used in the tests. *
 ********************************************************/

#define ERR                                                                    \
  fprintf(stderr,                                                              \
          RED " [test failed] " RESET "file:%s, line:%i\n",                    \
          __FILE__,                                                            \
          __LINE__)

#define EXPECT_TRUE(EXP)                                                       \
  if ((EXP) != true) {                                                         \
    fprintf(stderr, RED " [test failed] " RESET "EXPECT_TRUE(%s)\n", #EXP);    \
    ERR;                                                                       \
    return false;                                                              \
  }

#define EXPECT_FALSE(EXP)                                                      \
  if ((EXP) != false) {                                                        \
    fprintf(stderr, RED " [test failed] " RESET "EXPECT_FALSE(%s)\n", #EXP);   \
    ERR;                                                                       \
    return false;                                                              \
  }

/* Print information about the test. */
#define INFO(...)                                                              \
  printf(YELLOW " [info] " RESET __VA_ARGS__);                                 \
  printf("\n");

#endif /* !__TEST_H__ */
