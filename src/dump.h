#ifndef _DUMP_H_
#define _DUMP_H_

#include <time.h>

#include <nfc/nfc.h>

#include "freefare.h"
#include "main.h"

//------------------------------------------------------------------//
//                              Macros                              //
//------------------------------------------------------------------//

#define UID_SIZE       4
#define SECTOR_BLOCK_N 4
#define MAX_BLOCK_N 64

#define DUMP_HEADER_SIZE                                                                 \
  (sizeof(dump_t) - sizeof(MifareClassicBlock (*) [SECTOR_BLOCK_N]))
#define DUMP_CONTENT_SIZE(dump)                                                          \
  ((dump)->number_of_sectors * sizeof(MifareClassicBlock[SECTOR_BLOCK_N]))
#define DUMP_SIZE(dump) (DUMP_HEADER_SIZE + DUMP_CONTENT_SIZE((dump)))

#define FOPENR(f, file)                                                                  \
  {                                                                                      \
    if ((f = fopen((file), "rx")) == NULL) {                                             \
      return -1;                                                                         \
    }                                                                                    \
  }
#define FOPENW(f, file)                                                                  \
  {                                                                                      \
    if ((f = fopen((file), "wx")) == NULL) {                                             \
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
#define FWRITE(what, size, f)                                                            \
  {                                                                                      \
    if (fwrite((what), (size), 1, (f)) != 1) {                                           \
      fclose((f));                                                                       \
      return -1;                                                                         \
    }                                                                                    \
  }
#define FREAD(where, size, f)                                                            \
  {                                                                                      \
    if (fread((where), (size), 1, (f)) != 1) {                                           \
      fclose((f));                                                                       \
      return -1;                                                                         \
    }                                                                                    \
  }

//------------------------------------------------------------------//
//                            Constants                             //
//------------------------------------------------------------------//

static const char mhnac[] = "mhnac";

//------------------------------------------------------------------//
//                              Types                               //
//------------------------------------------------------------------//

typedef struct dump {
  char mhnac[sizeof(mhnac)];
  time_t creation_time;
  uint8_t uid[UID_SIZE];
  uint8_t number_of_sectors;
	struct {
		MifareClassicBlock (*formatted)[SECTOR_BLOCK_N];
		MifareClassicBlock *raw;
	} data;
} dump_t;

//------------------------------------------------------------------//
//                      Function declarations                       //
//------------------------------------------------------------------//

int
write_dump(dump_t *const dump, const char *const fname);

int
read_dump(dump_t *const dump, const char *const fname);

void
init_dump(dump_t *const dump, const char *const uid, const uint8_t number_of_sectors);

void
free_dump(dump_t *dump);

#endif // !_DUMP_H_
