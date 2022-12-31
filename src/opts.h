#ifndef _OPTS_H_
#define _OPTS_H_

#include <getopt.h>

#include "help.h"
#include "main.h"

typedef struct g_opts {
	char *desired_device;
	char *output_loc;
	char *input_loc;
	void (*fun)(g_state_t*);
} g_opts_t;

void
parse_user_flags(int argc, char *argv[], g_opts_t *g_opts);

#endif // !_OTPS_H_
