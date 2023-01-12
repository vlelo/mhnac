#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dump.h"
#include "freefare.h"
#include "main.h"
#include "utils.h"

//------------------------------------------------------------------//
//                      Function declarations                       //
//------------------------------------------------------------------//

__inline__ int
AUTH(const g_opts_t *const restrict G_opts,
     const MifareTag tag,
     const MifareClassicBlockNumber block,
     const MifareClassicKeyType key_type);

//------------------------------------------------------------------//
//                       Funciton definitions                       //
//------------------------------------------------------------------//

/**
 * @brief Writes in `&dest` `szBytes` bytes of binary data, given by the hexadecimal
 *rapresentatoin of `hex`
 *
 * @param `dest` Pointer to the destination buffer
 * @param `hex` String containing an hexadecimal formatted bytes.
 *						- Must be composed of multiples of two characters.
 *						- There must be no leading `0x`
 * @param `szBytes` Number of bytes to consider (two characters in hex)
 */
void
hex2bin(uint8_t *const dest, const char *hex, const size_t szBytes)
{
  for (register size_t count = 0; count < szBytes; count++) {
    sscanf(hex, "%2hhx", &dest[count]);
    hex += 2;
  }
}

/**
 * @brief Writes in `&dest` the hexadecimal rapresentation of the first `szBytes` bytes
 *of `bin`
 *
 * @param `dest` Pointer to the destination buffer. Must also have one extra byte for the
 *null terminator
 * @param `bin` Buffer containing the binary data
 * @param `szBytes` Number of bytes to consider
 */
void
bin2hex(char *const dest, const uint8_t *const bin, const size_t szBytes)
{
  for (register size_t count = 0; count < szBytes; count++) {
    sprintf(&dest[2 * count], "%02hhX", bin[count]);
  }
  dest[szBytes * 2] = '\0';
}

/**
 * @brief Injects the dump file blocks into free sectors of the card
 *
 * @param `G_state`
 * @param `G_opts`
 */
void
inject_dump(g_state_t *const restrict G_state, g_opts_t *const restrict G_opts)
{
  dump_t dump;
  int key_index;
  size_t block_index_card, block_index_dump;
  size_t n_key_sector;

  READ_DUMP(&dump, G_opts->input_loc);

  for (register long i = 0; i < dump.number_of_sectors; i++) {
    _AUTH(key_index,
          G_opts,
          G_state->tag,
          (i + dump.number_of_sectors) * SECTOR_BLOCK_N,
          MFC_KEY_A)

    for (register size_t j = 0; j < SECTOR_BLOCK_N - 1; j++) {
      block_index_card = (i + dump.number_of_sectors) * SECTOR_BLOCK_N + j;
      block_index_dump = i * SECTOR_BLOCK_N + j;
      mifare_classic_authenticate(
        G_state->tag, block_index_card, G_opts->keys[key_index], MFC_KEY_A);
      WRITE(G_state->tag, block_index_card, dump.data.raw[block_index_dump])
    }
  }

  n_key_sector = (dump.number_of_sectors / SECTOR_BLOCK_N) + 1;
  for (register size_t i = 0; i < n_key_sector; i++) {
    _AUTH(key_index,
          G_opts,
          G_state->tag,
          (i + 2 * dump.number_of_sectors) * SECTOR_BLOCK_N,
          MFC_KEY_A)

    for (register long j = 0, sector_key_counter = 0;
         j < SECTOR_BLOCK_N - 1 && sector_key_counter < dump.number_of_sectors;
         j++, sector_key_counter++)
    {
      block_index_card = (i + 2 * dump.number_of_sectors) * SECTOR_BLOCK_N + j;
      block_index_dump = (sector_key_counter + 1) * SECTOR_BLOCK_N;
      mifare_classic_authenticate(
        G_state->tag, block_index_card, G_opts->keys[key_index], MFC_KEY_A);
      WRITE(G_state->tag, block_index_card, dump.data.raw[block_index_dump])
    }
  }

  free_dump(&dump);
}

/**
 * @brief Transfers data from injected blocks into value blocks
 *
 * @param `G_state`
 * @param `G_opts`
 */
void
transfer_credit(g_state_t *const restrict G_state, g_opts_t *const restrict G_opts)
{
  int src_key_index, dest_key_index;
  size_t src_index, dest_index;

  retreive_keys(G_state, G_opts);

  for (register long i = 0; i < G_opts->number_of_sectors; i++) {
    _AUTH(src_key_index,
          G_opts,
          G_state->tag,
          (i + G_opts->number_of_sectors) * SECTOR_BLOCK_N,
          MFC_KEY_A)
    _AUTH(dest_key_index, G_opts, G_state->tag, i * SECTOR_BLOCK_N, MFC_KEY_A)

    for (register size_t j = 0; j < SECTOR_BLOCK_N - 1; j++) {
      src_index = (i + G_opts->number_of_sectors) * SECTOR_BLOCK_N + j;
      dest_index = i * SECTOR_BLOCK_N + j;

      mifare_classic_authenticate(
        G_state->tag, src_index, G_opts->keys[src_key_index], MFC_KEY_A);
      RESTORE(G_state->tag, src_index);
      mifare_classic_authenticate(
        G_state->tag, dest_index, G_opts->keys[dest_key_index], MFC_KEY_A);
      TRANSFER(G_state->tag, dest_index);
    }
  }
}

/**
 * @brief Performs `inject_block`, `transfer_credit` and `cean_card` in order
 *
 * @param `G_state`
 * @param `G_opts`
 */
void
recharge_card(g_state_t *const restrict G_state, g_opts_t *const restrict G_opts)
{
  inject_dump(G_state, G_opts);
  transfer_credit(G_state, G_opts);
  clean_card(G_state, G_opts);
}

/**
 * @brief Dumps the value blocks of the card
 *
 * @param `G_state`
 * @param `G_opts`
 */
void
dump_card(g_state_t *const restrict G_state, g_opts_t *const restrict G_opts)
{
  dump_t dump;
  int key_index;
  size_t block_index;
  init_dump(&dump, freefare_get_tag_uid(G_state->tag), G_opts->number_of_sectors);

  for (register long i = 0; i < G_opts->number_of_sectors; i++) {
    _AUTH(key_index, G_opts, G_state->tag, i * SECTOR_BLOCK_N, MFC_KEY_A)
    for (register size_t j = 0; j < SECTOR_BLOCK_N; j++) {
      block_index = i * SECTOR_BLOCK_N + j;
      mifare_classic_authenticate(
        G_state->tag, block_index, G_opts->keys[key_index], MFC_KEY_A);
      READ(G_state->tag, block_index, &dump.data.raw[block_index])
    }
  }

  if (G_opts->output_loc == NULL) {
    G_opts->output_loc = malloc(sizeof(char) * (4 * 2 + 5)); // 4*2 = uid hex + 5 ".mhnac"
    strcpy(G_opts->output_loc, freefare_get_tag_uid(G_state->tag));
    strcat(G_opts->output_loc, ".mhnac");
  }

  WRITE_DUMP(&dump, G_opts->output_loc);

  free_dump(&dump);
}

/**
 * @brief Sets to 0 all injected blocks
 *
 * @param `G_state`
 * @param `G_opts`
 */
void
clean_card(g_state_t *const restrict G_state, g_opts_t *const restrict G_opts)
{
  MifareClassicBlock zero = {0};
  int key_index;
  size_t block_index;
  size_t n_key_sector = (G_opts->number_of_sectors / SECTOR_BLOCK_N) + 1;

  for (register size_t i = 0; i < G_opts->number_of_sectors + n_key_sector; i++) {
    _AUTH(key_index,
          G_opts,
          G_state->tag,
          (i + G_opts->number_of_sectors) * SECTOR_BLOCK_N,
          MFC_KEY_A)

    for (register size_t j = 0; j < SECTOR_BLOCK_N - 1; j++) {
      block_index = (i + G_opts->number_of_sectors) * SECTOR_BLOCK_N + j;
      mifare_classic_authenticate(
        G_state->tag, block_index, G_opts->keys[key_index], MFC_KEY_A);
      WRITE(G_state->tag, block_index, zero)
    }
  }

  RESTORE(G_state->tag, block_index);
}

/**
 * @brief Prints the given dump
 *
 * @param `G_state`
 * @param `G_opts`
 */
void
print_dump(g_state_t *const restrict G_state, g_opts_t *const restrict G_opts)
{
  dump_t dump;
  char buf[BLOCK_SIZE_CHAR + 1];

  READ_DUMP(&dump, G_opts->input_loc);

  printf("======== mhnac header ========\n\n");
  bin2hex(buf, dump.uid, sizeof(dump.uid));
  printf("UID: %s\n", buf);
  printf("Created on: %s\n", ctime(&dump.creation_time));

  printf("========= mhnac body =========\n\n");
  for (register uint8_t i = 0; i < dump.number_of_sectors; i++) {
    for (register size_t j = 0; j < SECTOR_BLOCK_N - 1; j++) {
      bin2hex(buf, dump.data.raw[(i * SECTOR_BLOCK_N) + j], BLOCK_SIZE);
      printf("%s\n", buf);
    }
    bin2hex(buf, dump.data.raw[(i * SECTOR_BLOCK_N) + 3], BLOCK_SIZE);
    printf(RED "%.*s" YEL "%.*s" GRN "%.*s" RESET,
           KEY_SIZE_CHAR,
           buf,
           ACCESS_SIZE_CHAR,
           &buf[KEY_SIZE_CHAR],
           KEY_SIZE_CHAR,
           &buf[ACCESS_SIZE_CHAR + KEY_SIZE_CHAR]);
    printf("\n\n");
  }

  free_dump(&dump);
}

/**
 * @brief If keys are not provided by the user, they are retrieved from the injected
 * blocks
 *
 * @param `G_state`
 * @param `G_opts`
 */
void
retreive_keys(g_state_t *const restrict G_state, g_opts_t *const restrict G_opts)
{
  MifareClassicBlock buf;
  int key_index;
  size_t n_key_sector = (G_opts->number_of_sectors / SECTOR_BLOCK_N) + 1;
  size_t block_index;

  for (register size_t i = 0; i < n_key_sector; i++) {
    _AUTH(key_index,
          G_opts,
          G_state->tag,
          (i + 2 * G_opts->number_of_sectors) * SECTOR_BLOCK_N,
          MFC_KEY_A)

    for (register long j = 0, sector_key_counter = 0;
         j < SECTOR_BLOCK_N - 1 && sector_key_counter < G_opts->number_of_sectors;
         j++, sector_key_counter++)
    {
      block_index = (i + 2 * G_opts->number_of_sectors) * SECTOR_BLOCK_N + j;
      mifare_classic_authenticate(
        G_state->tag, block_index, G_opts->keys[key_index], MFC_KEY_A);
      READ(G_state->tag, block_index, &buf)

      G_opts->n_keys++;
      G_opts->keys = (MifareClassicKey *) realloc(
        G_opts->keys, (sizeof(MifareClassicKey) * G_opts->n_keys));
      memcpy(G_opts->keys[G_opts->n_keys - 1], buf, KEY_SIZE);
    }
  }
}

/**
 * @brief Automatically authenticates to the card by cycling through the list of keys
 *
 * @param `G_opts`
 * @param `tag` Tag to authenticate to
 * @param `block` Block to authenticate to
 * @param `key_type` Key type
 * @return Status:
 *					- 0 on success
 *					- \-1 on failure
 */
__inline__ int
AUTH(const g_opts_t *const restrict G_opts,
     const MifareTag tag,
     const MifareClassicBlockNumber block,
     const MifareClassicKeyType key_type)
{
  for (register size_t i = 0; i < G_opts->n_keys; i++) {
    if (mifare_classic_authenticate(tag, block, G_opts->keys[i], key_type) == 0) {
      return i;
    }
  }
  return -1;
}
