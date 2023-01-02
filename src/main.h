#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>

#include <nfc/nfc.h>
#include <sys/cdefs.h>
#include <freefare.h>

//------------------------------------------------------------------//
//                              Macros                              //
//------------------------------------------------------------------//

#define RGBFG(r, g, b) "\x1b[38;2;" #r ";" #g ";" #b "m"
#define RGBBG(r, g, b) "\x1b[48;2;" #r ";" #g ";" #b "m"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define BOLD  "\x1B[1m"
#define ITAL  "\x1B[3m"
#define UNDE  "\x1B[4m"

#define __FREE_ALL                                                                       \
  {                                                                                      \
			if (G_state.context)                                                               \
				nfc_exit(G_state.context);                                                       \
			if (G_state.pnd)                                                                   \
				nfc_close(G_state.pnd);                                                          \
			if (G_state.tags)                                                                  \
				freefare_free_tags(G_state.tags);                                                \
			if (G_opts.desired_device)                                                         \
				free(G_opts.desired_device);                                                     \
			if (G_opts.input_loc)                                                              \
				free(G_opts.input_loc);                                                          \
			if (G_opts.output_loc)                                                             \
				free(G_opts.output_loc);                                                         \
  }
#define __FREE_ALL_ptr                                                                   \
	{                                                                                      \
			if (G_state->context)                                                              \
				nfc_exit(G_state->context);                                                      \
			if (G_state->pnd)                                                                  \
				nfc_close(G_state->pnd);                                                         \
			if (G_state->tags)                                                                 \
				freefare_free_tags(G_state->tags);                                               \
			if (G_opts->desired_device)                                                        \
				free(G_opts->desired_device);                                                    \
			if (G_opts->input_loc)                                                             \
				free(G_opts->input_loc);                                                         \
			if (G_opts->output_loc)                                                            \
				free(G_opts->output_loc);                                                        \
	}
#ifndef DEBUG
#define __ERROR(msg, ...)                                                                \
  {                                                                                      \
    fprintf(stderr, RED "ERROR: " RESET msg "\n", __VA_ARGS__);                          \
  }
#define __WARN(msg, ...)                                                                 \
  {                                                                                      \
    fprintf(stderr, YEL "WARNING: " RESET msg "\n", __VA_ARGS__);                        \
  }
#
#define __dprint(msg, ...)
#define __dlprint(msg, ...)
#define __inline__ static inline
#
#else
#
#define __ERROR(msg, ...)                                                                \
  {                                                                                      \
    fprintf(stderr,                                                                      \
            YEL "%s:%d " RED "ERROR: " RESET msg "\n",                                   \
            __FILE__,                                                                    \
            __LINE__,                                                                    \
            __VA_ARGS__);                                                                \
  }
#define __WARN(msg, ...)                                                                 \
  {                                                                                      \
    fprintf(stderr,                                                                      \
            YEL "%s:%d " YEL "WARNING: " RESET msg "\n",                                 \
            __FILE__,                                                                    \
            __LINE__,                                                                    \
            __VA_ARGS__);                                                                \
  }
#
#define __dprint(msg, ...)                                                               \
  {                                                                                      \
    fprintf(stderr, CYN "DEBUG: " RESET msg "\n", __VA_ARGS__);                          \
  }
#define __dlprint(msg, ...)                                                              \
  {                                                                                      \
    fprintf(stderr,                                                                      \
            YEL "%s:%d " CYN "DEBUG: " RESET msg "\n",                                   \
            __FILE__,                                                                    \
            __LINE__,                                                                    \
            __VA_ARGS__);                                                                \
  }
#
#define __inline__
#endif

#define __PANIC(err, msg, ...)                                                           \
  {                                                                                      \
    __ERROR(msg, __VA_ARGS__);                                                            \
    __FREE_ALL;                                                                          \
    exit(err);                                                                           \
  }
#define __PANIC_ptr(err, msg, ...)                                                       \
  {                                                                                      \
    __ERROR(msg, __VA_ARGS__);                                                           \
    __FREE_ALL_ptr;                                                                      \
    exit(err);                                                                           \
  }
#define __PANIC_NFC(err, pnd)                                                            \
  {                                                                                      \
    __PANIC(err, "%s", nfc_strerror(pnd));                                               \
  }
#define __PANIC_NFC_ptr(err, pnd)                                                        \
  {                                                                                      \
    __PANIC_ptr(err, "%s", nfc_strerror(pnd));                                           \
  }
#define __PANIC_FF(err, pnd)                                                             \
  {                                                                                      \
    __PANIC(err, "%s", freefare_strerror(pnd));                                          \
  }
#define __PANIC_FF_ptr(err, pnd)                                                         \
  {                                                                                      \
    __PANIC_ptr(err, "%s", freefare_strerror(pnd));                                      \
  }

#define __mf_anticollision(nt, pnd)                                                      \
  {                                                                                      \
    if (nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &nt) < 0) {          \
      __ERROR("Tag has been removed", NULL);                                             \
      __PANIC_NFC(EXIT_FAILURE, pnd);                                                    \
    }                                                                                    \
  }

//------------------------------------------------------------------//
//                         Global constants                         //
//------------------------------------------------------------------//

/* poll for ISO14443A (MIFARE CLASSIC tag) */
static const nfc_modulation nmMifare = {
  .nmt = NMT_ISO14443A,
  .nbr = NBR_106,
};

typedef struct g_state {
	nfc_context *context;
	nfc_device *pnd;
	MifareTag tag;
	MifareTag *tags;
} g_state_t;

#define KEY_SIZE 6

//------------------------------------------------------------------//
//                      Function declarations                       //
//------------------------------------------------------------------//

#endif // _MAIN_H_
