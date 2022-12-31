#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <nfc/nfc.h>
#include <string.h>

#include "keys.h"
#include "main.h"
#include "mifare.h"
#include "opts.h"
#include "utils.h"

static g_state_t G_state;

static g_opts_t G_opts;

int
main(int argc, char *argv[])
{
	/* Exit if no argment provided */
	if (argc == 1) {
		__PRINT_HELP;
		exit(EXIT_FAILURE);
	}
	parse_user_flags(argc, argv, &G_opts);

	printf("" RED "█████ " GRN "█████ " YEL "█████ " BLU "█████ " MAG "█████ " CYN "█████ " WHT "█████ " RESET "█████\n");

	/* print welcome message */
  const char *acLibnfcVersion = nfc_version();
  printf(MAG "%s" RESET " uses libnfc " GRN "%s\n", "mhnac", acLibnfcVersion);

	{ /* Initialize libnfc */
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
		/* set opened nfc device as reader */
		if (nfc_initiator_init(G_state.pnd) < 0) {
			__PANIC_NFC(EXIT_FAILURE, G_state.pnd);
		}
		printf("NFC reader: `%s` opened succesfully\n", nfc_device_get_name(G_state.pnd));
	}

	{ /* Set device settings */
		/* drop the field for a while, so can be reset */
		if (nfc_device_set_property_bool(G_state.pnd, NP_ACTIVATE_FIELD, true) < 0) {
			__PANIC_NFC(EXIT_FAILURE, G_state.pnd);
		}
		/* let the reader only try once to find a tag */
		if (nfc_device_set_property_bool(G_state.pnd, NP_INFINITE_SELECT, false) < 0) {
			__PANIC_NFC(EXIT_FAILURE, G_state.pnd);
		}
		/* configure the CRC and Parity settings */
		if (nfc_device_set_property_bool(G_state.pnd, NP_HANDLE_CRC, true) < 0) {
			__PANIC_NFC(EXIT_FAILURE, G_state.pnd);
		}
		if (nfc_device_set_property_bool(G_state.pnd, NP_HANDLE_PARITY, true) < 0) {
			__PANIC_NFC(EXIT_FAILURE, G_state.pnd);
		}
	}

  /* establish contact with tag */
	size_t tag_count;
  if ((tag_count = nfc_initiator_select_passive_target(G_state.pnd, nmMifare, NULL, 0, &G_state.nt) < 0)) {
    __PANIC_NFC(EXIT_FAILURE, G_state.pnd);
  } else if (tag_count == 0) {
		__PANIC(EXIT_FAILURE, "No tag found", NULL);
  }

  /* test if a compatible MIFARE tag is used */
  if (((G_state.nt.nti.nai.btSak & 0x08) == 0) && (G_state.nt.nti.nai.btSak != 0x01)) {
		__PANIC(EXIT_FAILURE, "The found tag is not mifare. Only mifare tags are supported", NULL);
  }

  // print_tag_info(&G_state);
	G_opts.fun(&G_state);

  /* close device */
  nfc_close(G_state.pnd);
  /* close libnfc */
  nfc_exit(G_state.context);

  exit(EXIT_SUCCESS);
}
