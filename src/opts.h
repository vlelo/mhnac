#ifndef _OPTS_H_
#define _OPTS_H_

#include <getopt.h>

#include "help.h"

typedef struct g_opts {
	char *desired_device;
} g_opts_t;

void
parse_user_flags(int argc, char *argv[], g_opts_t *g_opts);

#endif // !_OTPS_H_
