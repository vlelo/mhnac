#ifndef _DUMP_H_
#define _DUMP_H_

#include <time.h>

#include <nfc/nfc.h>

#include "main.h"

//------------------------------------------------------------------//
//                              Macros                              //
//------------------------------------------------------------------//

#define DUMP_SIZE sizeof(dump_t)

#define FOPENR(f, file)                                                                  \
  {                                                                                      \
    if ((f = fopen((file), "rx")) == NULL) {                                             \
      return -1;                                                                         \
    }                                                                                    \
  }
#define FOPENW(f, file)                                                                  \
  {                                                                                      \
    if ((f = fopen((file), "Wx")) == NULL) {                                             \
      return -1;                                                                         \
    }                                                                                    \
  }

#define READ_DUMP(dump, fname)                                                           \
  {                                                                                      \
    int res = read_dump((dump), (fname));                                                \
    if (res == -1) {                                                                     \
      __PANIC_ptr(EXIT_FAILURE,                                                          \
                  "Could not perform IO on file " BOLD "`%s`" RESET ": %s",              \
                  fname,                                                                 \
                  strerror(errno));                                                      \
    } else if (res == -2) {                                                              \
      __PANIC_ptr(EXIT_FAILURE,                                                          \
                  "File " BOLD "`%s`" RESET " is not a valid " MAG "mhnac" RESET         \
                  " dump",                                                               \
                  (fname));                                                              \
    }                                                                                    \
  }
#define WRITE_DUMP(dump, fname)                                                          \
  {                                                                                      \
    if (write_dump((dump), (fname)) < 0) {                                               \
      __PANIC_ptr(EXIT_FAILURE,                                                          \
                  "Could not perform IO on file " BOLD "`%s`" RESET ": %s",              \
                  (fname),                                                               \
                  strerror(errno));                                                      \
    }                                                                                    \
  }

//------------------------------------------------------------------//
//                            Constants                             //
//------------------------------------------------------------------//

static const char binid[] = "mhnac";

//------------------------------------------------------------------//
//                              Types                               //
//------------------------------------------------------------------//

typedef struct dump {
  char id[6];
  time_t tm;
  uint8_t uid[4];
  union {
    struct {
      MifareClassicBlock data[4];
      MifareClassicBlock mistery[4];
    } ordered;
    MifareClassicBlock buffer[8];
  } data;
} dump_t;

//------------------------------------------------------------------//
//                      Function declarations                       //
//------------------------------------------------------------------//

int
write_dump(dump_t *dump, char *fname);

int
read_dump(dump_t *const dump, const char *const fname);

#endif // !_DUMP_H_
