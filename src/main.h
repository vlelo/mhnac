#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>

#include <nfc/nfc.h>

#include "nfc-utils.h"

//------------------------------------------------------------------//
//                              Macros                              //
//------------------------------------------------------------------//

#define RGBFG(r, g, b) "\x1b[38;2;"#r";"#g";"#b"m"
#define RGBBG(r, g, b) "\x1b[48;2;"#r";"#g";"#b"m"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#ifndef DEBUG
# define __PANIC(err, msg, ...) {fprintf(stderr, RED "ERROR: " RESET msg "\n", __VA_ARGS__); exit(err);}
# define __WARN(msg, ...) {fprintf(stderr, YEL "WARNING: " RESET msg "\n", __VA_ARGS__);}
#else
#	define __PANIC(err, msg, ...) {fprintf(stderr, YEL "%s:%d " RED "ERROR: " RESET msg "\n", __FILE__, __LINE__, __VA_ARGS__); exit(err);}
#	define __WARN(msg, ...) {fprintf(stderr, YEL "%s:%d " YEL "WARNING: " RESET msg "\n", __FILE__, __LINE__, __VA_ARGS__);}
#endif

#define __PANIC_NFC(err, pnd) {__PANIC(err, "%s", nfc_strerror(pnd));}

//------------------------------------------------------------------//
//                      Function declarations                       //
//------------------------------------------------------------------//

static inline void print_tag_info(nfc_target nt);

//------------------------------------------------------------------//
//                   Inline function definitions                    //
//------------------------------------------------------------------//

static inline void print_tag_info(nfc_target nt)
{
  printf("The following (NFC) ISO14443A tag was found:\n");
  printf("    ATQA (SENS_RES): ");
  print_hex(nt.nti.nai.abtAtqa, 2);
  printf("       UID (NFCID%c): ",
         (nt.nti.nai.abtUid[0] == 0x08 ? '3' : '1'));
  print_hex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
  printf("      SAK (SEL_RES): ");
  print_hex(&nt.nti.nai.btSak, 1);
  if (nt.nti.nai.szAtsLen) {
    printf("          ATS (ATR): ");
    print_hex(nt.nti.nai.abtAts, nt.nti.nai.szAtsLen);
  }
}

#endif // _MAIN_H_
