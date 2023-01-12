#include <errno.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dump.h"
#include "freefare.h"
#include "help.h"
#include "main.h"
#include "opts.h"
#include "utils.h"

static const struct option longopts[] = {
  {"help",      no_argument,       0, 'h'},
  {"inject",    required_argument, 0, 'J'},
  {"transfer",  no_argument,       0, 'T'},
  {"recharge",  required_argument, 0, 'R'},
  {"dump",      no_argument,       0, 'D'},
  {"clean",     no_argument,       0, 'C'},
  {"print",     required_argument, 0, 'P'},
  {"out",       required_argument, 0, 'o'},
  {"device",    required_argument, 0, 'd'},
  {"key",       required_argument, 0, 'k'},
  {"key-file",  required_argument, 0, 'f'},
  {"n-sectors", required_argument, 0, 'n'},
  {0,           0,                 0, 0  },
};

// static const char *optstring = "abc:d:012";

static const char *optstring = ":hJ:TR:DCP:o:d:k:f:n:b";

/**
 * @brief Sets the global program options by parsing user arguments
 *
 * @param `argc`
 * @param `argv`
 * @param `G_opts`
 */
void
parse_user_flags(const int argc, char *const argv[], g_opts_t *const restrict G_opts)
{
  opterr = 0; // don't use default getopt errors
  char **strotl_err = NULL;
  int c;
  while ((c = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
    switch (c) {
    case 'h':
      __PRINT_HELP;
      break;
    case 'J':
      MULTI_CMD;
      G_opts->input_loc = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
      strncpy(G_opts->input_loc, optarg, sizeof(char) * (strlen(optarg) + 1));
      G_opts->fun = inject_dump;
      break;
    case 'T':
      MULTI_CMD;
      G_opts->fun = transfer_credit;
      break;
    case 'R':
      MULTI_CMD;
      G_opts->input_loc = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
      strncpy(G_opts->input_loc, optarg, sizeof(char) * (strlen(optarg) + 1));
      G_opts->fun = recharge_card;
      break;
    case 'D':
      MULTI_CMD;
      G_opts->fun = dump_card;
      break;
    case 'C':
      MULTI_CMD;
      G_opts->fun = clean_card;
      break;
    case 'P':
      MULTI_CMD;
      G_opts->input_loc = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
      strncpy(G_opts->input_loc, optarg, sizeof(char) * (strlen(optarg) + 1));
      G_opts->fun = print_dump;
      break;
    case 'o':
      G_opts->output_loc = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
      strncpy(G_opts->output_loc, optarg, sizeof(char) * (strlen(optarg) + 1));
      break;
    case 'd':
      G_opts->desired_device = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
      strncpy(G_opts->desired_device, optarg, sizeof(char) * (strlen(optarg) + 1));
      break;
    case 'k':
      keys_from_stdin(optarg, G_opts);
      break;
    case 'f':
      G_opts->key_file_opts.input_loc =
        (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
      strncpy(
        G_opts->key_file_opts.input_loc, optarg, sizeof(char) * (strlen(optarg) + 1));
      break;
    case 'n':
      G_opts->number_of_sectors = strtol(optarg, strotl_err, 10);

      if (G_opts->number_of_sectors <= 0) {
        if (strotl_err) {
          __ERROR("Invalid argument to number of sectors: " F_STR, optarg)
          FREE_OPTS_ptr exit(EXIT_FAILURE);
        }
        __ERROR("Number of sectors must be positive non zero", NULL)
        FREE_OPTS_ptr exit(EXIT_FAILURE);
      } else if (G_opts->number_of_sectors > 6) {
        __ERROR("Number of sectors must be at most " CYN "6" RESET, NULL)
        FREE_OPTS_ptr exit(EXIT_FAILURE);
      }
      break;
    case 'b':
      G_opts->key_file_opts.bin = true;
      break;
    case '?': // unknown option
      __ERROR("invalid option " F_STR, argv[optind - 1]);
      FREE_OPTS_ptr exit(EXIT_FAILURE);
      break;
    case ':': // missing option argument
      __ERROR("option " F_STR " requires an argument", argv[optind - 1]);
      FREE_OPTS_ptr exit(EXIT_FAILURE);
      break;
    default:
      FREE_OPTS_ptr fprintf(stderr, "Ambiguous option? %s -%c", argv[optind - 1], optopt);
      exit(EXIT_FAILURE);
    };
  };
}

/**
 * @brief Reads colon separated (':') keys from `optarg`
 *
 * @param `optarg`
 * @param `G_opts`
 */
void
keys_from_stdin(const char *restrict optarg, g_opts_t *const restrict G_opts)
{
  int count = 0;
  size_t n_keys = 0;
  char c = 1;

  G_opts->keys = (MifareClassicKey *) malloc(1);

  while (c != '\0') {
    c = *(optarg + count);

    switch (c) {
    case '\0':
    case ':':
      if (count != KEY_SIZE_CHAR) {
        __ERROR("Invalid key size: " CYN "%.*s" RESET, count, optarg)
        FREE_OPTS_ptr exit(EXIT_FAILURE);
      }
      n_keys++;
      G_opts->keys =
        (MifareClassicKey *) realloc(G_opts->keys, (sizeof(MifareClassicKey) * n_keys));
      hex2bin(G_opts->keys[n_keys - 1], optarg, KEY_SIZE);
      optarg += KEY_SIZE_CHAR + 1; // +1 is :
      count = 0;
      break;
    default:
      count++;
      break;
    }
  }
  G_opts->n_keys = n_keys;
}

/**
 * @brief Read keys form file:
 *						- if `G_opts->key_file_opts.bin == true` the file is read as binary
 *						- if `G_opts->key_file_opts.bin == false` the file is read as a newline
 *separated ('\n') list of keys
 *
 * @param `G_opts`
 */
void
keys_from_file(g_opts_t *const restrict G_opts)
{
  FILE *f;

  MifareClassicKey buffer;
  size_t btCount;

  char strBuffer[KEY_SIZE_CHAR];
  char c = 1;
  int count = 0;
  size_t line_n = 0;

  if (G_opts->keys == NULL) {
    G_opts->keys = (MifareClassicKey *) malloc(1);
  }

  if (G_opts->key_file_opts.bin) {
    if ((f = fopen(G_opts->key_file_opts.input_loc, "rx")) == NULL) {
      FREE_OPTS_ptr __ERROR("%s", strerror(errno));
      exit(EXIT_FAILURE);
    }

    while (!feof(f)) {
      if ((btCount = fread(&buffer, 1, sizeof(MifareClassicKey), f)) != KEY_SIZE) {
        __WARN("Invaild last key in file " YEL "%s" RESET ": " CYN "%ld" RESET
               " extra bytes",
               G_opts->key_file_opts.input_loc,
               btCount)
        break;
      }
      G_opts->n_keys++;
      G_opts->keys = (MifareClassicKey *) realloc(
        G_opts->keys, (sizeof(MifareClassicKey) * G_opts->n_keys));
      memcpy(G_opts->keys[G_opts->n_keys - 1], buffer, KEY_SIZE);
    }
  } else {
    if ((f = fopen(G_opts->key_file_opts.input_loc, "r")) == NULL) {
      FREE_OPTS_ptr __ERROR("%s", strerror(errno));
      exit(EXIT_FAILURE);
    }

    while (!feof(f)) {
      if ((c = getc(f)) == EOF) {
        break;
      }

      switch (c) {
      case EOF:
      case '\n':
        if (count < KEY_SIZE_CHAR) {
          __ERROR("Invalid key size in file " YEL "%s" RESET
                  ": key too short on line " CYN "%ld" RESET,
                  G_opts->key_file_opts.input_loc,
                  line_n)
          FREE_OPTS_ptr exit(EXIT_FAILURE);
        }
        G_opts->n_keys++;
        G_opts->keys = (MifareClassicKey *) realloc(
          G_opts->keys, (sizeof(MifareClassicKey) * G_opts->n_keys));
        hex2bin(G_opts->keys[G_opts->n_keys - 1], strBuffer, KEY_SIZE);
        count = 0;
        line_n++;
        break;
      default:
        if (count > KEY_SIZE_CHAR) {
          __ERROR("Invalid key size in file " YEL "%s" RESET
                  ": leading characters on line " CYN "%ld" RESET,
                  G_opts->key_file_opts.input_loc,
                  line_n)
          FREE_OPTS_ptr exit(EXIT_FAILURE);
        }
        strBuffer[count] = c;
        count++;
        break;
      }
    }
  }

  fclose(f);
}
