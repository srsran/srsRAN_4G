// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom
#ifndef SRSRAN_PARITY_H_
#define SRSRAN_PARITY_H_

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

#endif /* SRSRAN_PARITY_H_ */
