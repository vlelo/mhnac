#include <stdio.h>
#include <stdlib.h>

#include <nfc/nfc.h>

#include "main.h"

static void
print_hex(const uint8_t *pbtData, const size_t szBytes)
{
  size_t  szPos;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  char *desired_device = NULL;

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
  printf("NFC reader: %s opened succesfully\n",
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

	printf("The following (NFC) ISO14443A tag was found:\n");
	printf("    ATQA (SENS_RES): ");
	print_hex(nt.nti.nai.abtAtqa, 2);
	printf("       UID (NFCID%c): ", (nt.nti.nai.abtUid[0] == 0x08 ? '3' : '1'));
	print_hex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
	printf("      SAK (SEL_RES): ");
	print_hex(&nt.nti.nai.btSak, 1);
	if (nt.nti.nai.szAtsLen) {
		printf("          ATS (ATR): ");
		print_hex(nt.nti.nai.abtAts, nt.nti.nai.szAtsLen);
	}

	/* close device */
  nfc_close(pnd);
  /* close libnfc */
  nfc_exit(context);

  return 0;
}
