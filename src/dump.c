#include <stdio.h>
#include <string.h>

#include "dump.h"

/**
 * @brief Write dump object to file
 *
 * @param `dump` Dump object to be written
 * @param `fname` File path
 * @return Status of the operation:
 *					- 0 success
 *					- \-1 IO error
 */
int
write_dump(dump_t *const restrict dump, const char *const restrict fname)
{
  FILE *f;

  FOPENW(f, fname);

  dump->tm = time(NULL);
  memcpy(dump->id, binid, sizeof(binid));

  if (fwrite(dump, sizeof(dump_t), 1, f) != DUMP_SIZE) {
    fclose(f);
    return -1;
  }
  fclose(f);

  return 0;
}

/**
 * @brief Read a dump object from a file
 *
 * @param dump Buffer to store the dump object
 * @param fname File path
 * @return Status of the operation:
 *						- 0 success
 *						- \-1 IO error
 *						- \-2 invalid file
 */
int
read_dump(dump_t *const restrict dump, const char *const restrict fname)
{
  FILE *f;

  FOPENR(f, fname);
  fseek(f, 0L, SEEK_END);
  if (ftell(f) != DUMP_SIZE) {
    fclose(f);
    return -2;
  }
  rewind(f);

  if (fread(dump, sizeof(dump_t), 1, f) != DUMP_SIZE) {
    fclose(f);
    return -1;
  }
  fclose(f);

  if (memcmp(dump, binid, sizeof(binid))) {
    return -2;
  }
  return 0;
}
