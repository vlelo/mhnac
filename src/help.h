#ifndef _HELP_H_
#define _HELP_H_

#include "main.h"

static const char *help_text =
"Usage:\n"
"      mhnac command [OPTION]    Display current tag information"
	"\n"
	"\v"
"Description:\n"
"      Mi   Hpiacciono Nle     Acose Cgratis"
	"\n"
"      Mnon Hovogliadi Npagare Alle  Cmacchinette"
	"\n"
	"\v"
"Commands:\n"
"      --help,     -h                Display this message"
	"\n"
"      --inject,   -J <dump>         Inject <dump> into card"
	"\n"
"      --transfer, -T                Transfer credit from hidden buffer to data sectors"
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
"      --key,      -k <key>          Use <key> as A key for the data blocks. Only mandatory for --dump, otherwise will try to use the one on bin file."
  "\n"
"      --null-key, -n <key>          Use <key> as A key for the injection blocks. Defaults to [0]"
  "\n"
;

#define __PRINT_HELP             \
	{                              \
		fputs(help_text, stdout);    \
    exit(EXIT_SUCCESS);          \
	}

#endif
