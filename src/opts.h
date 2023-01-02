#ifndef _OPTS_H_
#define _OPTS_H_

#include <getopt.h>

#include "help.h"
#include "main.h"

#define MULTI_CMD { \
	if (G_opts->fun != NULL) {                                       \
		__ERROR("Multiple commands specified", NULL)                   \
		exit(EXIT_FAILURE);                                            \
	}                                                                \
}

typedef struct g_opts {
	char *desired_device;
	char *output_loc;
	char *input_loc;
	// uint8_t key[KEY_SIZE];
	MifareClassicKey key;
	void (*fun)(g_state_t*, struct g_opts*);
} g_opts_t;

void
parse_user_flags(int argc, char *argv[], g_opts_t *g_opts);

#endif // !_OTPS_H_
