#ifndef _HELP_H_
#define _HELP_H_

#include "main.h"

static const char *help_text =
"Usage:\n"
"      mhnac command [OPTIONS]          Display current tag information"
	"\n"
	"\v"
"Description:\n"
"      Mi   Hpiacciono Nle     Acose Cgratis"
	"\n"
"      Mnon Hovogliadi Npagare Alle  Cmacchinette"
	"\n"
	"\v"
"Commands:\n"
"      --help,        -h                Display this message"
	"\n"
"      --inject,      -J <dump>         Inject <dump> file into card free sectors"
	"\n"
"      --transfer,    -T                Transfer credit from hidden sectors to data sectors of the card"
	"\n"
"      --recahrge,    -R <dump>         Inject, transfer and clean card with <dump>"
	"\n"
"      --dump,        -D                Dump the card in mhnac format"
	"\n"
"      --clean,       -C                Clean injected sectors"
	"\n"
"      --print,       -P <dump>         Print info of <dump>"
	"\n"
	"\v"
"Options:\n"
"      --out,         -o <dest>         Write dump to <dest>"
  "\n"
"      --device,      -d <dev>          Use <dev> as NFC device"
  "\n"
"      --key,         -k <keys>         Add <keys> to the key list of the program\n"
"                                       <keys> is a colon (':') separated list of A keys\n"
"                                       mhnac will automatically fetch keys for all operations that aren't --dump\n"
	"\n"
"      --key-file,    -f <file>         Add keys to the key list ot the program, reading form <file>\n"
"                                       <file> is a newline ('\\n') separated list of keys, unless the -b flag is given\n"
	"\n"
"                     -b                <file> of --key-file is treated as a binary file, where a key is every 6 bytes\n"
	"\n"
"      --n-sectors,   -n <uint>         <uint> is the number of data sectors mhnac has to consider\n"
"                                       Must be a positive integer between 1 and 6 (included)\n"
"                                       Default value is 4\n"
	"\n"
;

#define __PRINT_HELP             \
	{                              \
		fputs(help_text, stdout);    \
    exit(EXIT_SUCCESS);          \
	}

#endif
