#ifndef _OPTS_H_
#define _OPTS_H_

#include <freefare.h>

#include "main.h"

//------------------------------------------------------------------//
//                              Macros                              //
//------------------------------------------------------------------//

#define MULTI_CMD                                                                        \
  {                                                                                      \
    if (G_opts->fun != NULL) {                                                           \
      __ERROR("Multiple commands specified", NULL)                                       \
      exit(EXIT_FAILURE);                                                                \
    }                                                                                    \
  }

//------------------------------------------------------------------//
//                              Types                               //
//------------------------------------------------------------------//

typedef struct g_opts {
  char *desired_device;
  char *output_loc;
  char *input_loc;
  MifareClassicKey key_data;
  MifareClassicKey key_mistery;
  MifareClassicKey null_key;
  bool key_d_set;
  bool key_m_set;
  void (*fun)(g_state_t *, struct g_opts *);
} g_opts_t;

//------------------------------------------------------------------//
//                      Function declarations                       //
//------------------------------------------------------------------//

void
parse_user_flags(int argc, char *const argv[], g_opts_t *const g_opts);

#endif // !_OTPS_H_
