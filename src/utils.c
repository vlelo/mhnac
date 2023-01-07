#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "dump.h"
#include "freefare.h"
#include "main.h"
#include "utils.h"

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
    sprintf(&dest[2*count], "%02hhX", bin[count]);
  }
	dest[szBytes*2] = '\0';
}

/**
 * @brief Injects the dump file blocks into free sectors of the card
 *
 * @param `G_state`
 * @param `G_opts`
 */
void
inject_block(g_state_t *const restrict G_state, g_opts_t *const restrict G_opts)
{
  dump_t dump;

  READ_DUMP(&dump, G_opts->input_loc);

  for (register size_t i = 0; i < 3; i++) {
    AUTH(G_state->tag, i + INJECT_STRIDE, G_opts->null_key, MFC_KEY_A);
    WRITE(G_state->tag, i + INJECT_STRIDE, dump.data.ordered.data[i]);
  }
  for (register size_t i = 0; i < 3; i++) {
    AUTH(G_state->tag, i + INJECT_STRIDE + NEXT, G_opts->null_key, MFC_KEY_A);
    WRITE(G_state->tag, i + INJECT_STRIDE + NEXT, dump.data.ordered.mistery[i]);
  }
  AUTH(G_state->tag, INJECT_STRIDE + 2 * NEXT, G_opts->null_key, MFC_KEY_A);
  WRITE(G_state->tag, INJECT_STRIDE + 2 * NEXT, dump.data.ordered.data[3]);
  AUTH(G_state->tag, INJECT_STRIDE + 2 * NEXT + 1, G_opts->null_key, MFC_KEY_A);
  WRITE(G_state->tag, INJECT_STRIDE + 2 * NEXT + 1, dump.data.ordered.mistery[3]);
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
  retreive_keys(G_state, G_opts);

  for (register size_t i = 0; i < 3; i++) {
    AUTH(G_state->tag, i + INJECT_STRIDE, G_opts->null_key, MFC_KEY_A);
    RESTORE(G_state->tag, i + INJECT_STRIDE);
    AUTH(G_state->tag, i + CREDIT_STRIDE, G_opts->key_data, MFC_KEY_A);
    TRANSFER(G_state->tag, i + CREDIT_STRIDE);
  }
  for (register size_t i = 0; i < 3; i++) {
    AUTH(G_state->tag, i + INJECT_STRIDE + NEXT, G_opts->null_key, MFC_KEY_A);
    RESTORE(G_state->tag, i + INJECT_STRIDE + NEXT);
    AUTH(G_state->tag, i + CREDIT_STRIDE + NEXT, G_opts->key_data, MFC_KEY_A);
    TRANSFER(G_state->tag, i + CREDIT_STRIDE + NEXT);
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
  inject_block(G_state, G_opts);
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
  for (register size_t i = 0; i < 3; i++) {
    AUTH(G_state->tag, i + CREDIT_STRIDE, G_opts->key_data, MFC_KEY_A);
    READ(G_state->tag, i + CREDIT_STRIDE, &dump.data.ordered.data[i]);
  }
  for (register size_t i = 0; i < 3; i++) {
    AUTH(G_state->tag, i + CREDIT_STRIDE + NEXT, G_opts->key_mistery, MFC_KEY_A);
    READ(G_state->tag, i + CREDIT_STRIDE + NEXT, &dump.data.ordered.mistery[i]);
  }

  if (G_opts->output_loc == NULL) {
    G_opts->output_loc = malloc(sizeof(char) * (4 * 2 + 4)); // 4*2 = uid hex + 4 ".bin"
    strcpy(G_opts->output_loc, freefare_get_tag_uid(G_state->tag));
    strcat(G_opts->output_loc, ".bin");
  }

  hex2bin(dump.uid, freefare_get_tag_uid(G_state->tag), sizeof(dump.uid));

  WRITE_DUMP(&dump, G_opts->output_loc);
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

  for (register size_t i = 0; i < 12; i++) {
    if (!i % 3) {
      continue;
    }
    AUTH(G_state->tag, i + INJECT_STRIDE, G_opts->null_key, MFC_KEY_A);
    WRITE(G_state->tag, i + INJECT_STRIDE, zero);
  }
  RESTORE(G_state->tag, INJECT_STRIDE);
}

/**
 * @brief If keys are not provided by the user, they are retrieved from the injected blocks
 *
 * @param `G_state`
 * @param `G_opts`
 */
void
retreive_keys(g_state_t *const restrict G_state, g_opts_t *const restrict G_opts)
{
  MifareClassicBlock buf;

  if (!G_opts->key_d_set) {
    AUTH(G_state->tag, KEY_STRIDE, G_opts->null_key, MFC_KEY_A);
    READ(G_state->tag, KEY_STRIDE, &buf);
    memcpy(G_opts->key_data, buf, KEY_SIZE);
  }
  if (!G_opts->key_m_set) {
    AUTH(G_state->tag, KEY_STRIDE + 1, G_opts->null_key, MFC_KEY_A);
    READ(G_state->tag, KEY_STRIDE + 1, &buf);
    memcpy(G_opts->key_mistery, buf, KEY_SIZE);
  }
}
