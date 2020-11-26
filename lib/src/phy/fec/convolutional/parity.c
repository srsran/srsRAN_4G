/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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
