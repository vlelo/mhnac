#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <nfc/nfc.h>

#include "freefare.h"
#include "keys.h"
#include "main.h"
#include "opts.h"
#include "utils.h"

static g_state_t G_state;

static g_opts_t G_opts;

static void sigHandler(int signum);
__inline__ char *sig(int signum);

int
main(int argc, char *argv[])
{
	/* Exit if no argment provided */
	if (argc == 1) {
		__PRINT_HELP;
		exit(EXIT_FAILURE);
	}
	parse_user_flags(argc, argv, &G_opts);

	nfc_init(&G_state.context);
	if (G_state.context == NULL) {
		__PANIC(EXIT_FAILURE, "Unable to init libnfc (malloc)", NULL);
	}

	/* open nfc device */
	G_state.pnd = nfc_open(G_state.context, G_opts.desired_device);
	if (G_state.pnd == NULL) {
		__PANIC(EXIT_FAILURE,
						"Unable to open NFC device "
						BOLD "`%s`" RESET,
						(G_opts.desired_device ? G_opts.desired_device : "default"));
	}

	if (signal(SIGINT, sigHandler) != 0) {
		__dprint("Could not attach signal handler to SIGINT", NULL);
	}
	if (signal(SIGTERM, sigHandler) != 0) {
		__dprint("Could not attach signal handler to SIGTERM", NULL);
	}
	if (signal(SIGQUIT, sigHandler) != 0) {
		__dprint("Could not attach signal handler to SIGQUIT", NULL);
	}
	if (signal(SIGKILL, sigHandler) != 0) {
		__dprint("Could not attach signal handler to SIGKILL", NULL);
	}

  /* establish contact with first tag */
	if ((G_state.tags = freefare_get_tags(G_state.pnd)) == NULL) {
		__PANIC(EXIT_FAILURE, "Could not find any tag", NULL);
	}
	// size_t found_tags_n = 0;
	// for (register MifareTag *p = G_state.tags; p != NULL; p += sizeof(MifareTag)) {
	// 	found_tags_n++;
	// }
	G_state.tag = G_state.tags[0];
	printf("Connecting to tag with UID: %s\n", freefare_get_tag_uid(G_state.tag));
	
	if (freefare_get_tag_type(G_state.tag) != CLASSIC_1K || freefare_get_tag_type(G_state.tag) != CLASSIC_4K) {
		__PANIC(EXIT_FAILURE, "Tag is not mifare. Found tag is %s.", freefare_get_tag_friendly_name(G_state.tag));
	}

	if (mifare_classic_connect(G_state.tag) < 0) {
		__PANIC_FF(EXIT_FAILURE, G_state.tag);
	}

	G_opts.fun(&G_state, &G_opts);

	if (mifare_classic_disconnect(G_state.tag) < 0) {
		__PANIC_FF(EXIT_FAILURE, G_state.tag);
	}

	/* free all found tags */
	freefare_free_tags(G_state.tags);
  /* close device */
  nfc_close(G_state.pnd);
  /* close libnfc */
  nfc_exit(G_state.context);

  exit(EXIT_SUCCESS);
}

static void sigHandler(int signum)
{
	putc('\n', stdout);
	__PANIC(EXIT_FAILURE, "Exiting: signal "RED"(%s)"RESET, sig(signum));
}

__inline__ char *sig(int signum)
{
	if (signum == 2) {
		return "SIGINT";
	}
	if (signum == 3) {
		return "SIGQUIT";
	}
	if (signum == 9) {
		return "SIGKILL";
	}
	if (signum == 15) {
		return "SIGTERM";
	}
	return "UNKNOWN";
}
