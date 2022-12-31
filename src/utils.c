#include "utils.h"
#include "keys.h"
#include "main.h"
#include "nfc-utils.h"
#include "nfc/nfc-types.h"

/**
 * @brief Utility function to print information on read tag
 *
 * @param G_state <g_state_t> the target tag
 * @return nil
 */
void
print_tag_info(g_state_t *G_state)
{
	nfc_target nt = G_state->nt;
  printf("The following (NFC) ISO14443A tag was found:\n");
  printf("    ATQA (SENS_RES): ");
  print_hex(nt.nti.nai.abtAtqa, 2);
  printf("      UID (NFCID%c): ", (nt.nti.nai.abtUid[0] == 0x08 ? '3' : '1'));
  print_hex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
  printf("      SAK (SEL_RES): ");
  print_hex(&nt.nti.nai.btSak, 1);
  if (nt.nti.nai.szAtsLen) {
    printf("          ATS (ATR): ");
    print_hex(nt.nti.nai.abtAts, nt.nti.nai.szAtsLen);
  }
}

void
inject_block(g_state_t *G_state)
{
  mifare_param param;
  struct mifare_param_auth key;
  memcpy(key.abtKey, common_keys[0], sizeof(key.abtKey));
  memcpy(key.abtAuthUid, G_state->nt.nti.nai.abtUid, sizeof(key.abtAuthUid));
  param.mpa = key;

  if (!nfc_initiator_mifare_cmd(G_state->pnd, MC_AUTH_A, 62, &param)) {
    __PANIC_ptr(EXIT_FAILURE, "Could not authenticate", NULL);
  }
  __WARN("Authenticated succesfully", NULL)

	__PANIC_ptr(EXIT_FAILURE, "Not implemented", NULL);
}

void
recharge_card(g_state_t *G_state)
{
	__PANIC_ptr(EXIT_FAILURE, "Not implemented", NULL);
}
