#ifndef _HELP_H_
#define _HELP_H_

#include "main.h"

static const char *help_text =
"Usage:\n"
"      mhnac            Display current tag information"
	"\n"
"      mhnac --help     Display this message"
	"\n"
;

#define __PRINT_HELP             \
	{                              \
		fputs(help_text, stdout);    \
    exit(EXIT_SUCCESS);          \
	}

#endif
