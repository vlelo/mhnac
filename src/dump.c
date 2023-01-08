#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dump.h"
#include "util.h"
#include "utils.h"

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
	for (register size_t i; i < dump->number_of_sectors * SECTOR_BLOCK_N; i++) {
		FWRITE(dump->data.raw[i], sizeof(MifareClassicBlock), f)
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
	for (register size_t i; i < dump->number_of_sectors * SECTOR_BLOCK_N; i++) {
		FREAD(dump->data.raw[i], sizeof(MifareClassicBlock), f)
	}

  return 0;
}

/**
 * @brief Initialize a `dump_t` dump and allocate it's space
 *
 * @param `dump` Dump to be initialized
 * @param `uid` UID in string format
 * @param `number_of_sectors` Number of sectors the dump should hold
 */
void
init_dump(dump_t *const restrict dump, const char *const restrict uid, const uint8_t number_of_sectors)
{
  memcpy(dump->mhnac, mhnac, sizeof(mhnac));
  dump->creation_time = time(NULL);
  hex2bin(dump->uid, uid, sizeof(dump->uid));
	dump->number_of_sectors = number_of_sectors;
	dump->data.raw = malloc(sizeof(MifareClassicBlock) * number_of_sectors * SECTOR_BLOCK_N);
	dump->data.formatted = (MifareClassicBlock (*)[SECTOR_BLOCK_N]) dump->data.raw;
}

/**
 * @brief Free allocated space of a `dump_t` dump
 *
 * @param `dump` Dump to be freed
 */
void
free_dump(dump_t * dump)
{
	free(dump->data.raw);
}
