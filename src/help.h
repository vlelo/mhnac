#ifndef _HELP_H_
#define _HELP_H_

#include "main.h"

static const char *help_text =
"Usage:\n"
"      mhnac command [OPTION]    Display current tag information"
	"\n"
	"\v"
"Commands:\n"
"      --help,     -h                Display this message"
	"\n"
"      --inject,   -J <dump>         Inject <dump> into card"
	"\n"
"      --recahrge, -R <dump>         Recharge card with <dump> (calls also inject and clean)"
	"\n"
"      --dump,     -D                Dump the card"
	"\n"
"      --clean,    -C                Clean injected sectors"
	"\n"
	"\v"
"Options:\n"
"      --out,      -o <dest>         Write dump to <dest>"
  "\n"
"      --device,   -d <dev>          Use <dev> NFC device"
  "\n"
"      --key,      -k <key>          Use <key> as A key for tag"
  "\n"
;

#define __PRINT_HELP             \
	{                              \
		fputs(help_text, stdout);    \
    exit(EXIT_SUCCESS);          \
	}

#endif
