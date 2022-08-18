#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <nfc/nfc.h>

#include "main.h"
#include "mifare.h"
#include "keys.h"

int main(int argc, char *argv[])
{
  (void) argc;

  char *desired_device = NULL;

  const char *acLibnfcVersion = nfc_version();
  printf(
    MAG "%s" RESET " uses libnfc %s\n", argv[0], acLibnfcVersion);

  /* initialize libnfc */
  nfc_context *context;
  nfc_init(&context);
  if (context == NULL) {
    __PANIC(EXIT_FAILURE, "Unable to init libnfc (malloc)", NULL);
  }

  /* open nfc device */
  nfc_device *pnd = nfc_open(context, desired_device);
  if (pnd == NULL) {
    __PANIC(EXIT_FAILURE,
            "Unable to open NFC device "
            "`%s`",
            (desired_device ? desired_device : "default"));
  }
  /* set opened nfc device as reader */
  if (nfc_initiator_init(pnd) < 0) {
    __PANIC_NFC(EXIT_FAILURE, pnd);
  }
  printf("NFC reader: `%s` opened succesfully\n",
         nfc_device_get_name(pnd));

  /* establish contact with tag */
  nfc_target nt;
  /* poll for ISO14443A (MIFARE CLASSIC tag) */
  const nfc_modulation nmMifare = {
    .nmt = NMT_ISO14443A,
    .nbr = NBR_106,
  };
  if (nfc_initiator_select_passive_target(
        pnd, nmMifare, NULL, 0, &nt) < 0)
  {
    __PANIC_NFC(EXIT_FAILURE, pnd);
  }

	print_tag_info(nt);

	mifare_param param;
	struct mifare_param_auth key = {
		.abtKey = common_keys[0][0],
		.abtAuthUid = *nt.nti.nai.abtUid,
	};
	param.mpa = key;

	if (!nfc_initiator_mifare_cmd(pnd, MC_AUTH_A, 62, &param)) {
		__PANIC(EXIT_FAILURE, "Could not authenticate", NULL);
	}
	__WARN("Authenticated succesfully", NULL)

  /* close device */
  nfc_close(pnd);
  /* close libnfc */
  nfc_exit(context);

	exit(EXIT_SUCCESS);
}
