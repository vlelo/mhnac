#ifndef _UTILS_H_
#define _UTILS_H_

#include <nfc/nfc.h>

#include "mifare.h"
#include "main.h"

void
print_tag_info(g_state_t *G_state);

void
inject_block(g_state_t *G_state);

void
recharge_card(g_state_t *G_state);

#endif /* !_UTILS_H_ */
