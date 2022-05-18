#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "options.h"

enum {
  CASE_LEN,
  CASE_LANG,
  CASE_FIRST,
};

static struct option long_options[] = {
  { "len", required_argument, 0, 0 },
  { "lang", required_argument, 0, 0 },
  { "first", required_argument, 0, 0 },
  { 0, 0, 0,  0 }
};

static enum lang options_map_lang(const char *lang)
{
  if (strcmp(lang, "en") == 0) {
    return EN;
  }
  if (strcmp(lang, "fr") == 0) {
    return FR;
  }
  assert (!"lang not supported");
}

static void options_set_default(struct options *opts)
{
  opts->len = DEFAULT_LEN;
  opts->lang = DEFAULT_LANG;
  opts->first = NULL;
}

void options_parse(int argc, char **argv, struct options *opts)
{
  int c;

  options_set_default(opts);

  while (true) {
    int option_index = 0;
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (option_index) {
      case CASE_LEN:
        opts->len = atoi(optarg);
        break;
      case CASE_LANG:
        opts->lang = options_map_lang(optarg);
        break;
      case CASE_FIRST:
        opts->first = strdup(optarg);
        break;
    }
  }
}

void options_clean(struct options *opts)
{
  free(opts->first);
}
