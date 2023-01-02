#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "dump.h"
#include "freefare.h"
#include "main.h"
#include "utils.h"

void
hex2bin(uint8_t *const dest, const char *hex, const size_t szBytes)
{
  for (register size_t count = 0; count < szBytes; count++) {
    sscanf(hex, "%2hhx", &dest[count]);
    hex += 2;
  }
}

void
print_hex(const uint8_t *pbtData, const size_t szBytes)
{
  size_t szPos;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}

void
print_hex_bits(const uint8_t *pbtData, const size_t szBits)
{
  uint8_t uRemainder;
  size_t szPos;
  size_t szBytes = szBits / 8;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }

  uRemainder = szBits % 8;
  // Print the rest bits
  if (uRemainder != 0) {
    if (uRemainder < 5)
      printf("%01x (%d bits)", pbtData[szBytes], uRemainder);
    else
      printf("%02x (%d bits)", pbtData[szBytes], uRemainder);
  }
  printf("\n");
}

void
inject_block(g_state_t *G_state, g_opts_t *G_opts)
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

void
transfer_credit(g_state_t *G_state, g_opts_t *G_opts)
{
  set_keys_if_unset(G_state, G_opts);

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

void
recharge_card(g_state_t *G_state, g_opts_t *G_opts)
{
  inject_block(G_state, G_opts);
  transfer_credit(G_state, G_opts);
  clean_card(G_state, G_opts);
}

void
dump_card(g_state_t *G_state, g_opts_t *G_opts)
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

void
clean_card(g_state_t *G_state, g_opts_t *G_opts)
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

void
set_keys_if_unset(g_state_t *G_state, g_opts_t *G_opts)
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
