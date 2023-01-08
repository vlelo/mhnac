#ifndef _OPTS_H_
#define _OPTS_H_

#include <freefare.h>

#include "main.h"

//------------------------------------------------------------------//
//                              Macros                              //
//------------------------------------------------------------------//

#define FREE_OPTS                                                                        \
  {                                                                                      \
    if (G_opts->desired_device)                                                          \
      free(G_opts->desired_device);                                                      \
    if (G_opts->input_loc)                                                               \
      free(G_opts->input_loc);                                                           \
    if (G_opts->output_loc)                                                              \
      free(G_opts->output_loc);                                                          \
    if (G_opts->keys)                                                                    \
      free(G_opts->keys);                                                                \
    if (G_opts->key_file_opts.input_loc)                                                 \
      free(G_opts->key_file_opts.input_loc);                                             \
  }
#define MULTI_CMD                                                                        \
  {                                                                                      \
    if (G_opts->fun != NULL) {                                                           \
      __ERROR("Multiple commands specified", NULL)                                       \
      FREE_OPTS                                                                          \
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
  MifareClassicKey *keys;
  struct {
    char *input_loc;
    bool bin;
  } key_file_opts;
  size_t n_keys;
	long int number_of_sectors;
  void (*fun)(g_state_t *, struct g_opts *);
} g_opts_t;

//------------------------------------------------------------------//
//                      Function declarations                       //
//------------------------------------------------------------------//

void
parse_user_flags(int argc, char *const argv[], g_opts_t *const G_opts);

void
keys_from_stdin(const char *optarg, g_opts_t *const G_opts);

void
keys_from_file(g_opts_t *const G_opts);

#endif // !_OTPS_H_
