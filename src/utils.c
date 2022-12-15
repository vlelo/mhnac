#include "utils.h"
#include "nfc-utils.h"

/**
 * @brief Utility function to print information on read tag
 *
 * @param nt <nfc_target> the target tag
 * @return nil
 */
void
print_tag_info(nfc_target nt)
{
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
