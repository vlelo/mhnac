#include <stddef.h>
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

	/* HEADER */
	FWRITE(dump->mhnac, sizeof(dump->mhnac), f);
	FWRITE(&dump->creation_time, sizeof(dump->creation_time), f);
	FWRITE(dump->uid, sizeof(dump->uid), f);
	FWRITE(&dump->number_of_sectors, sizeof(dump->number_of_sectors), f);

	/* DATA */
	for (register size_t i; i < dump->number_of_sectors; i++) {
		FWRITE(dump->data[i], sizeof(MifareClassicBlock[SECTOR_BLOCK_N]), f)
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

	/* HEADER */
	FREAD(dump->mhnac, sizeof(dump->mhnac), f);
	FREAD(&dump->creation_time, sizeof(dump->creation_time), f);
	FREAD(dump->uid, sizeof(dump->uid), f);
	FREAD(&dump->number_of_sectors, sizeof(dump->number_of_sectors), f);

  if (memcmp(dump->mhnac, mhnac, sizeof(mhnac))) {
    return -2;
  }
	if (dump->number_of_sectors > 6 || dump->number_of_sectors <= 0) {
    return -2;
	}

	size_t pos = ftell(f);
  fseek(f, 0L, SEEK_END);
  if (ftell(f) != DUMP_SIZE(dump)) {
    fclose(f);
    return -2;
  }
	fseek(f, pos, SEEK_SET);

	/* DATA */
	for (register size_t i; i < dump->number_of_sectors; i++) {
		FREAD(dump->data[i], sizeof(MifareClassicBlock[SECTOR_BLOCK_N]), f)
	}

  return 0;
}
