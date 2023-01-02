#include "opts.h"
#include "main.h"
#include "utils.h"
#include <string.h>

static const struct option longopts[] = {
  { "help",     no_argument,       0, 'h' },
	{ "inject",   required_argument, 0, 'J' },
	{ "recharge", required_argument, 0, 'R' },
	{ "dump",     no_argument,       0, 'D' },
	{ "clean",    no_argument,       0, 'C' },
	{ "out",      required_argument, 0, 'o' },
	{ "device",   required_argument, 0, 'd' },
	{ "key",      required_argument, 0, 'k' },
  { 0,          0,                 0,  0  },
};

// static const char *optstring = "abc:d:012";

static const char *optstring = "hJ:R:Do:d:k:";

static const int *longindex = NULL;

void parse_user_flags(int argc, char *argv[], g_opts_t *G_opts)
{
	(void) G_opts;
	(void) longindex;

	int c;
  while ((c = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
		switch (c) {
			case 'h':
				__PRINT_HELP;
				break;
			case 'J':
				MULTI_CMD;
				G_opts->input_loc = (char*) malloc(sizeof(char) * (strlen(optarg) + 1));
				strncpy(G_opts->input_loc, optarg, sizeof(char) * (strlen(optarg) + 1));
				G_opts->fun = inject_block;
				break;
			case 'R':
				MULTI_CMD;
				G_opts->input_loc = (char*) malloc(sizeof(char) * (strlen(optarg) + 1));
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
				G_opts->output_loc = (char*) malloc(sizeof(char) * (strlen(optarg) + 1));
				strncpy(G_opts->output_loc, optarg, sizeof(char) * (strlen(optarg) + 1));
				break;
			case 'd':
				G_opts->desired_device = (char*) malloc(sizeof(char) * (strlen(optarg) + 1));
				strncpy(G_opts->desired_device, optarg, sizeof(char) * (strlen(optarg) + 1));
				break;
			case 'k':
				if (strlen(optarg) != 2*KEY_SIZE) {
					__ERROR("Invalid key size.", NULL);
					exit(EXIT_FAILURE);
				}
				hex2bin(G_opts->key, optarg, KEY_SIZE);
				break;
			default:
				exit(EXIT_FAILURE);
		};
	};
}
