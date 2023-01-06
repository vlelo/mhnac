#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "help.h"
#include "main.h"
#include "opts.h"
#include "utils.h"

static const struct option longopts[] = {
  {"help",        no_argument,       0, 'h'},
  {"inject",      required_argument, 0, 'J'},
  {"transfer",    no_argument,       0, 'T'},
  {"recharge",    required_argument, 0, 'R'},
  {"dump",        no_argument,       0, 'D'},
  {"clean",       no_argument,       0, 'C'},
  {"out",         required_argument, 0, 'o'},
  {"device",      required_argument, 0, 'd'},
  {"key-data",    required_argument, 0, 'k'},
  {"key-mistery", required_argument, 0, 'm'},
  {"null-key",    required_argument, 0, 'n'},
  {0,             0,                 0, 0  },
};

// static const char *optstring = "abc:d:012";

static const char *optstring = ":hJ:TR:DCo:d:k:m:n:";

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
      G_opts->fun = inject_block;
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
    case 'o':
      G_opts->output_loc = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
      strncpy(G_opts->output_loc, optarg, sizeof(char) * (strlen(optarg) + 1));
      break;
    case 'd':
      G_opts->desired_device = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
      strncpy(G_opts->desired_device, optarg, sizeof(char) * (strlen(optarg) + 1));
      break;
    case 'k':
      if (strlen(optarg) != 2 * KEY_SIZE) {
        __ERROR("Invalid key size", NULL);
        exit(EXIT_FAILURE);
      }
      G_opts->key_d_set = true;
      hex2bin(G_opts->key_data, optarg, KEY_SIZE);
      break;
    case 'm':
      if (strlen(optarg) != 2 * KEY_SIZE) {
        __ERROR("Invalid key size", NULL);
        exit(EXIT_FAILURE);
      }
      G_opts->key_m_set = true;
      hex2bin(G_opts->key_mistery, optarg, KEY_SIZE);
      break;
    case 'n':
      if (strlen(optarg) != 2 * KEY_SIZE) {
        __ERROR("Invalid key size for null-key", NULL);
        exit(EXIT_FAILURE);
      }
      hex2bin(G_opts->null_key, optarg, KEY_SIZE);
      break;
		case '?': // unknown option
			__ERROR("invalid option " BOLD "`%s`" RESET, argv[optind - 1]);
			exit(EXIT_FAILURE);
			break;
		case ':': // missing option argument
			__ERROR("option " BOLD "`%s`" RESET " requires an argument", argv[optind - 1]);
			exit(EXIT_FAILURE);
			break;
    default:
		fprintf(stderr, "Ambiguous option? %s -%c", argv[optind -1], optopt);
      exit(EXIT_FAILURE);
    };
  };
}
