#ifndef _UTILS_H_
#define _UTILS_H_

#include <time.h>

#include <nfc/nfc.h>

#include "dump.h"
#include "main.h"
#include "opts.h"

//------------------------------------------------------------------//
//                              Macros                              //
//------------------------------------------------------------------//

// #define AUTH(tag, block, key, type)                                                      \
//   {                                                                                      \
//     if (mifare_classic_authenticate((tag), (block), (key), (type)) < 0) {                \
//       __PANIC_FF_ptr(EXIT_FAILURE, (tag));                                               \
//     }                                                                                    \
//   }
// #define AUTH(tag, block, type)                                                           \
//   {                                                                                      \
//     for (register size_t i = 0; i < G_opts->n_keys; i++) {                               \
//       if (mifare_classic_authenticate((tag), (block), G_opts->keys[i], (type)) == 0) {   \
//         break;                                                                           \
//       }                                                                                  \
//     }                                                                                    \
//   }
#define _AUTH(ret, G_opts, tag, block, key_type)                                         \
  {                                                                                      \
    if (((ret) = AUTH((G_opts), (tag), (block), (key_type))) < 0) {                        \
      __PANIC_ptr(EXIT_FAILURE, "Could not authenticate to tag with any key", NULL)      \
    }                                                                                    \
  }
#define READ(tag, block, buf)                                                            \
  {                                                                                      \
    if (mifare_classic_read((tag), (block), (buf)) < 0) {                                \
      __PANIC_FF_ptr(EXIT_FAILURE, (tag));                                               \
    }                                                                                    \
  }
#define WRITE(tag, block, buf)                                                           \
  {                                                                                      \
    if (mifare_classic_write((tag), (block), (buf)) < 0) {                               \
      __PANIC_FF_ptr(EXIT_FAILURE, (tag));                                               \
    }                                                                                    \
  }
#define RESTORE(tag, block)                                                              \
  {                                                                                      \
    if (mifare_classic_restore((tag), (block)) < 0) {                                    \
      __PANIC_FF_ptr(EXIT_FAILURE, (tag));                                               \
    }                                                                                    \
  }
#define TRANSFER(tag, block)                                                             \
  {                                                                                      \
    if (mifare_classic_transfer((tag), (block)) < 0) {                                   \
      __PANIC_FF_ptr(EXIT_FAILURE, (tag));                                               \
    }                                                                                    \
  }

//------------------------------------------------------------------//
//                      Function declarations                       //
//------------------------------------------------------------------//

void
hex2bin(uint8_t *const dest, const char *hex, const size_t szBytes);

void
bin2hex(char *const dest, const uint8_t *const bin, const size_t szBytes);

void
inject_block(g_state_t *G_state, g_opts_t *G_opts);

void
transfer_credit(g_state_t *G_state, g_opts_t *G_opts);

void
recharge_card(g_state_t *G_state, g_opts_t *G_opts);

void
dump_card(g_state_t *G_state, g_opts_t *G_opts);

void
clean_card(g_state_t *G_state, g_opts_t *G_opts);

void
retreive_keys(g_state_t *G_state, g_opts_t *G_opts);

#endif /* !_UTILS_H_ */
