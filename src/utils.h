#ifndef _UTILS_H_
#define _UTILS_H_

#include <nfc/nfc.h>

#include "main.h"
#include "opts.h"

#define DATA_STRIDE 8
#define TRASFER_STRIDE 56
#define DUMP_SIZE 6
#define READ_AMOUNT 8

#define FOPENR(f, file) {                                                  \
	if ((f = fopen((file), "rx")) == NULL) {                                 \
		__PANIC_ptr(EXIT_FAILURE, "Could not read file " BOLD "`%s`"           \
		"" RESET ": %s", (file), strerror(errno));                             \
	}                                                                        \
}
#define FOPENW(f, file) {                                                  \
	if ((f = fopen((file), "Wx")) == NULL) {                                 \
		__PANIC_ptr(EXIT_FAILURE, "Could not open file " BOLD "`%s`"           \
		"" RESET ": %s", (file), strerror(errno));                             \
	}                                                                        \
}
#define AUTH(tag, block, key, type) {                                      \
	if (mifare_classic_authenticate((tag), (block), (key), (type)) < 0) {    \
		__PANIC_FF_ptr(EXIT_FAILURE, (tag));                                   \
	}                                                                        \
}
#define READ(tag, block, buf) {                                            \
	if (mifare_classic_read((tag), (block), (buf)) < 0) {                    \
		__PANIC_FF_ptr(EXIT_FAILURE, (tag));                                   \
	}                                                                        \
}
#define WRITE(tag, block, buf) {                                           \
	if (mifare_classic_write((tag), (block), (buf)) < 0) {                   \
		__PANIC_FF_ptr(EXIT_FAILURE, (tag));                                   \
	}                                                                        \
}
#define RESTORE(tag, block) {                                              \
	if (mifare_classic_restore((tag), (block)) < 0) {                        \
		__PANIC_FF_ptr(EXIT_FAILURE, (tag));                                   \
	}                                                                        \
}
#define TRANSFER(tag, block) {                                             \
	if (mifare_classic_transfer((tag), (block)) < 0) {                       \
		__PANIC_FF_ptr(EXIT_FAILURE, (tag));                                   \
	}                                                                        \
}

void
hex2bin(uint8_t * const dest, const char *hex, const size_t szBytes);

void
print_hex(const uint8_t *pbtData, const size_t szBytes);

void
print_hex_bits(const uint8_t *pbtData, const size_t szBits);

void
inject_block(g_state_t *G_state, g_opts_t *G_opts);

void
recharge_card(g_state_t *G_state, g_opts_t *G_opts);

void
dump_card(g_state_t *G_state, g_opts_t *G_opts);

void
clean_card(g_state_t *G_state, g_opts_t *G_opts);

#endif /* !_UTILS_H_ */
