#include "utils.h"
#include "freefare.h"
#include "keys.h"
#include "main.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

void
hex2bin(uint8_t * const dest, const char *hex, const size_t szBytes)
{
	for (register size_t count = 0; count < szBytes; count++) {
		sscanf(hex, "%2hhx", &dest[count]);
		hex += 2;
	}
}

void
print_hex(const uint8_t *pbtData, const size_t szBytes)
{
  size_t  szPos;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}

void
print_hex_bits(const uint8_t *pbtData, const size_t szBits)
{
  uint8_t uRemainder;
  size_t  szPos;
  size_t  szBytes = szBits / 8;

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
	MifareClassicBlock dump[8];
	uint8_t null_key[KEY_SIZE] = {0};
	FILE *f;
	
	FOPENR(f, G_opts->input_loc);

	if (fread(dump, sizeof(MifareClassicBlock), DUMP_SIZE, f) < sizeof(MifareClassicBlock) * DUMP_SIZE) {
		__PANIC_ptr(EXIT_FAILURE, "Invalid format on file " BOLD "`%s`" RESET ": %s", G_opts->input_loc, strerror(errno));
	}

	for (register size_t i = 0; i < 8; i++) {
		if (!i%3) {
			continue;
		}
		AUTH(G_state->tag, i+56, null_key, MFC_KEY_A);
		WRITE(G_state->tag, i+56, dump[i]);
	}

	fclose(f);
}

void
recharge_card(g_state_t *G_state, g_opts_t *G_opts)
{
	uint8_t null_key[KEY_SIZE] = {0};

	inject_block(G_state, G_opts);
	for (register size_t i = 0; i < 8; i++) {
		if (!i%3) {
			continue;
		}
		AUTH(G_state->tag, i+56, null_key, MFC_KEY_A);
		RESTORE(G_state->tag, i+56);
		AUTH(G_state->tag, i+8, G_opts->key, MFC_KEY_A);
		TRANSFER(G_state->tag, i+8);
	}
	clean_card(G_state, G_opts);
}

void
dump_card(g_state_t *G_state, g_opts_t *G_opts)
{
	MifareClassicBlock dump[DUMP_SIZE];
	for (register size_t i = 0; i < READ_AMOUNT; i++) {
		if (!i%3) {
			continue;
		}
		AUTH(G_state->tag, i+DATA_STRIDE, G_opts->key, MFC_KEY_A);
		READ(G_state->tag, i+DATA_STRIDE, &dump[i]);
		print_hex(dump[i], sizeof(MifareClassicBlock));
	}
	if (G_opts->output_loc == NULL) {
		G_opts->output_loc = malloc(sizeof(char) * (4 * 2 + 4)); // 4*2 = uid hex + 4 ".bin"
		strcpy(G_opts->output_loc, freefare_get_tag_uid(G_state->tag));
		strcat(G_opts->output_loc, ".bin");
	}

	FILE *f;
	FOPENW(f, G_opts->output_loc);
	if (fwrite(dump, sizeof(MifareClassicBlock), DUMP_SIZE, f) < sizeof(MifareClassicBlock) * DUMP_SIZE) {
		__PANIC_ptr(EXIT_FAILURE, "Invalid format on file " BOLD "`%s`" RESET ": %s", G_opts->output_loc, strerror(errno));
	}
	fclose(f);
}

void
clean_card(g_state_t *G_state, g_opts_t *G_opts)
{
	uint8_t null_key[KEY_SIZE] = {0};
	MifareClassicBlock zero = {0};

	for (register size_t i = 0; i < 8; i++) {
		if (!i%3) {
			continue;
		}
		AUTH(G_state->tag, i+56, null_key, MFC_KEY_A);
		WRITE(G_state->tag, i+56, zero);
	}
}
