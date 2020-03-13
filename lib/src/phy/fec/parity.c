/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
