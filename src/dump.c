#include <stdio.h>
#include <string.h>

#include "dump.h"

int
write_dump(dump_t *dump, char *fname)
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

int
read_dump(dump_t *const dump, const char *const fname)
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
