/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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