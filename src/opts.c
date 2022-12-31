#include "opts.h"
#include "main.h"
#include "utils.h"
#include <string.h>

static const struct option longopts[] = {
  { "help",     no_argument,       0, 'h' },
	{ "print",    no_argument,       0, 'p' },
	{ "inject",   required_argument, 0, 'j' },
	{ "recharge", optional_argument, 0, 'r' },
	{ "out",      required_argument, 0, 'o' },
  { 0,          0,                 0,  0  },
};

// static const char *optstring = "abc:d:012";

static const char *optstring = "hpj:r::o:";

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
			case 'p':
				G_opts->fun = print_tag_info;
				break;
			case 'j':
				G_opts->fun = inject_block;
				break;
			case 'r':
				G_opts->fun = recharge_card;
				break;
			case 'o':
				G_opts->output_loc = (char*) malloc(sizeof(char) * (strlen(optarg) + 1));
				strncpy(G_opts->output_loc, optarg, sizeof(char) * (strlen(optarg) + 1));
				break;
			default:
				exit(EXIT_FAILURE);
		};
	};
}
