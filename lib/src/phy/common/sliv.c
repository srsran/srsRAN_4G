/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/common/sliv.h"

void srsran_sliv_to_s_and_l(uint32_t N, uint32_t v, uint32_t* S, uint32_t* L)
{
  uint32_t low  = v % N;
  uint32_t high = v / N;
  if (high + 1 + low <= N) {
    *S = low;
    *L = high + 1;
  } else {
    *S = N - 1 - low;
    *L = N - high + 1;
  }
}

uint32_t srsran_sliv_from_s_and_l(uint32_t N, uint32_t S, uint32_t L)
{
  if ((L - 1) <= N / 2) {
    return N * (L - 1) + S;
  }
  return N * (N - L + 1) + (N - 1 - S);
}