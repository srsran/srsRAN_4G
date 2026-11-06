// SPDX-FileCopyrightText: Copyright (C) 2009 Phil Karn, KA9Q
// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <stdint.h>
#include <stdio.h>

uint8_t  Partab[256];
uint32_t P_init;

/* Create 256-entry odd-parity lookup table
 * Needed only on non-ia32 machines
 */
void partab_init(void)
{
  uint32_t i, cnt, ti;

  /* Initialize parity lookup table */
  for (i = 0; i < 256; i++) {
    cnt = 0;
    ti  = i;
    while (ti) {
      if (ti & 1)
        cnt++;
      ti >>= 1;
    }
    Partab[i] = cnt & 1;
  }
  P_init = 1;
}
