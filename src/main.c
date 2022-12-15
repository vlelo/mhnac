#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <nfc/nfc.h>
#include <string.h>

#include "keys.h"
#include "main.h"
#include "mifare.h"
#include "opts.h"

static struct g_state {
	nfc_context *context;
	nfc_device *pnd;
	nfc_target nt;
} g_state;

g_opts_t g_opts = {
	.desired_device = NULL,
};

int
main(int argc, char *argv[])
{
	parse_user_flags(argc, argv, &g_opts);

	printf("" RED "█████ " GRN "█████ " YEL "█████ " BLU "█████ " MAG "█████ " CYN "█████ " WHT "█████ " RESET "█████\n");

	/* print welcome message */
  const char *acLibnfcVersion = nfc_version();
  printf(MAG "%s" RESET " uses libnfc " GRN "%s\n", "mhnac", acLibnfcVersion);

	{ /* Initialize libnfc */
		nfc_init(&g_state.context);
		if (g_state.context == NULL) {
			__PANIC(EXIT_FAILURE, "Unable to init libnfc (malloc)", NULL);
		}

		/* open nfc device */
		g_state.pnd = nfc_open(g_state.context, g_opts.desired_device);
		if (g_state.pnd == NULL) {
			__PANIC(EXIT_FAILURE,
							"Unable to open NFC device "
							BOLD "`%s`" RESET,
							(g_opts.desired_device ? g_opts.desired_device : "default"));
		}
		/* set opened nfc device as reader */
		if (nfc_initiator_init(g_state.pnd) < 0) {
			__PANIC_NFC(EXIT_FAILURE, g_state.pnd);
		}
		printf("NFC reader: `%s` opened succesfully\n", nfc_device_get_name(g_state.pnd));
	}

	{ /* Set device settings */
		/* drop the field for a while, so can be reset */
		if (nfc_device_set_property_bool(g_state.pnd, NP_ACTIVATE_FIELD, true) < 0) {
			__PANIC_NFC(EXIT_FAILURE, g_state.pnd);
		}
		/* let the reader only try once to find a tag */
		if (nfc_device_set_property_bool(g_state.pnd, NP_INFINITE_SELECT, false) < 0) {
			__PANIC_NFC(EXIT_FAILURE, g_state.pnd);
		}
		/* configure the CRC and Parity settings */
		if (nfc_device_set_property_bool(g_state.pnd, NP_HANDLE_CRC, true) < 0) {
			__PANIC_NFC(EXIT_FAILURE, g_state.pnd);
		}
		if (nfc_device_set_property_bool(g_state.pnd, NP_HANDLE_PARITY, true) < 0) {
			__PANIC_NFC(EXIT_FAILURE, g_state.pnd);
		}
	}

  /* establish contact with tag */
	size_t tag_count;
  if ((tag_count = nfc_initiator_select_passive_target(g_state.pnd, nmMifare, NULL, 0, &g_state.nt) < 0)) {
    __PANIC_NFC(EXIT_FAILURE, g_state.pnd);
  } else if (tag_count == 0) {
		__PANIC(EXIT_FAILURE, "No tag found", NULL);
  }

  /* test if a compatible MIFARE tag is used */
  if (((g_state.nt.nti.nai.btSak & 0x08) == 0) && (g_state.nt.nti.nai.btSak != 0x01)) {
		__PANIC(EXIT_FAILURE, "The found tag is not mifare. Only mifare tags are supported", NULL);
  }

  print_tag_info(g_state.nt);

  mifare_param param;
  struct mifare_param_auth key;
  memcpy(key.abtKey, common_keys[0], sizeof(key.abtKey));
  memcpy(key.abtAuthUid, g_state.nt.nti.nai.abtUid, sizeof(key.abtAuthUid));
  param.mpa = key;

  if (!nfc_initiator_mifare_cmd(g_state.pnd, MC_AUTH_A, 62, &param)) {
    __PANIC(EXIT_FAILURE, "Could not authenticate", NULL);
  }
  __WARN("Authenticated succesfully", NULL)

  /* close device */
  nfc_close(g_state.pnd);
  /* close libnfc */
  nfc_exit(g_state.context);

  exit(EXIT_SUCCESS);
}
