#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "options.h"

static struct option long_options[] = {
  { "len", required_argument, 0,  0 },
  { "lang", required_argument, 0,  0 },
  { 0, 0, 0,  0 }
};

static enum lang map_lang(const char *lang)
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
      case 0:
        opts->len = atoi(optarg);
        break;
      case 1:
        opts->lang = map_lang(optarg);
        break;
    }
  }
}
