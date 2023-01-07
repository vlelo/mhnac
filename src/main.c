#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nfc/nfc.h>

#include "freefare.h"
#include "help.h"
#include "main.h"
#include "opts.h"
#include "utils.h"

//------------------------------------------------------------------//
//                             Globals                              //
//------------------------------------------------------------------//

static g_state_t G_state = {0};

static g_opts_t G_opts = {0};

//------------------------------------------------------------------//
//                      Function declarations                       //
//------------------------------------------------------------------//

static void
sigHandler(int signum);

__inline__ char *
sig(int signum);

//------------------------------------------------------------------//
//                               main                               //
//------------------------------------------------------------------//

int
main(int argc, char *argv[])
{
  /* Exit if no argment provided */
  if (argc == 1) {
    __PRINT_HELP;
    exit(EXIT_FAILURE);
  }
  parse_user_flags(argc, argv, &G_opts);
  if (G_opts.key_file_opts.input_loc) {
    keys_from_file(&G_opts);
  }
	G_opts.n_keys += 2;
	G_opts.keys = (MifareClassicKey *) realloc(G_opts.keys, (sizeof(MifareClassicKey) * G_opts.n_keys));
	memset(G_opts.keys[G_opts.n_keys - 2], 0x00, KEY_SIZE);
	memset(G_opts.keys[G_opts.n_keys - 1], 0xFF, KEY_SIZE);

  char buffer[13];
  for (register size_t i = 0; i < G_opts.n_keys; i++) {
    bin2hex(buffer, G_opts.keys[i], KEY_SIZE);
    printf("[%ld] 0x%s\n", i, buffer);
  }
  exit(EXIT_SUCCESS);

  nfc_init(&G_state.context);
  if (G_state.context == NULL) {
    __PANIC(EXIT_FAILURE, "Unable to init libnfc (malloc)", NULL);
  }

  /* open nfc device */
  G_state.pnd = nfc_open(G_state.context, G_opts.desired_device);
  if (G_state.pnd == NULL) {
    __PANIC(EXIT_FAILURE,
            "Unable to open NFC device " BOLD "`%s`" RESET,
            (G_opts.desired_device ? G_opts.desired_device : "default"));
  }

  { /* Signal handling */
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

  if (freefare_get_tag_type(G_state.tag) != CLASSIC_1K ||
      freefare_get_tag_type(G_state.tag) != CLASSIC_4K)
  {
    __PANIC(EXIT_FAILURE,
            "Tag is not mifare. Found tag is %s.",
            freefare_get_tag_friendly_name(G_state.tag));
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

/**
 * @brief Handler for unix signal
 *
 * @param `signum` Signal number
 */
static void
sigHandler(const int signum)
{
  putc('\n', stdout);
  __PANIC(EXIT_FAILURE, "Exiting: signal " RED "(%s)" RESET, sig(signum));
}

/**
 * @brief Convert signal number `signum` to known signal string
 *
 * @param `signum` Signal number
 * @return Literal string of the signal name
 */
__inline__ char *
sig(const int signum)
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
