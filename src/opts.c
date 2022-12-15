#include "opts.h"
#include "main.h"

// static const struct option longopts[] = {
//   {"add",     required_argument, 0, 0  },
//   {"append",  no_argument,       0, 0  },
//   {"delete",  required_argument, 0, 0  },
//   {"verbose", no_argument,       0, 0  },
//   {"create",  required_argument, 0, 'c'},
//   {"file",    required_argument, 0, 0  },
//   {0,         0,                 0, 0  }
// };

static const struct option longopts[] = {
  {"help", no_argument, 0, 'h' },
  {0,      0,           0,  0  },
};

// static const char *optstring = "abc:d:012";

static const char *optstring = "h";

static const int *longindex = NULL;

void parse_user_flags(int argc, char *argv[], g_opts_t *g_opts)
{
	(void) g_opts;
	(void) longindex;

	int c;
  while ((c = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
		switch (c) {
			case 'h':
				__PRINT_HELP;
				break;
			default:
				exit(EXIT_FAILURE);
		};
	};
}
