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

#ifndef SRSRAN_RA_HELPER_H
#define SRSRAN_RA_HELPER_H

#include "srsran/phy/utils/debug.h"
#include <stdint.h>

static inline void ra_helper_compute_s_and_l(uint32_t N, uint32_t v, uint32_t* S, uint32_t* L)
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

static int ra_helper_freq_type1(uint32_t N_bwp_size, uint32_t riv, srsran_sch_grant_nr_t* grant)
{
  uint32_t start = 0;
  uint32_t len   = 0;
  ra_helper_compute_s_and_l(N_bwp_size, riv, &start, &len);

  if (start + len > N_bwp_size) {
    ERROR("RIV 0x%x for BWP size %d resulted in freq=%d:%d", riv, N_bwp_size, start, len);
    return SRSRAN_ERROR;
  }

  for (uint32_t i = 0; i < start; i++) {
    grant->prb_idx[i] = false;
  }

  for (uint32_t i = start; i < start + len; i++) {
    grant->prb_idx[i] = true;
  }

  for (uint32_t i = start + len; i < SRSRAN_MAX_PRB_NR; i++) {
    grant->prb_idx[i] = false;
  }
  grant->nof_prb = len;

  return SRSRAN_SUCCESS;
}

#endif // SRSRAN_RA_HELPER_H
