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
#ifndef SRSLTE_PARITY_H_
#define SRSLTE_PARITY_H_

#include <inttypes.h>

#ifdef __x86_64__
#define __i386__
#endif

/* Determine parity of argument: 1 = odd, 0 = even */
#ifdef __i386__
static inline uint32_t parityb(uint8_t x)
{
  __asm__ __volatile__("test %1,%1;setpo %0" : "=q"(x) : "q"(x));
  return x;
}
#else
void partab_init();

static inline uint32_t parityb(uint8_t x)
{
  extern uint8_t  Partab[256];
  extern uint32_t P_init;
  if (!P_init) {
    partab_init();
  }
  return Partab[x];
}
#endif

static inline uint32_t parity(int x)
{
  /* Fold down to one byte */
  x ^= (x >> 16);
  x ^= (x >> 8);
  return parityb(x);
}

#endif /* SRSLTE_PARITY_H_ */
