/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "polar_interleaver_gold.h"
#include "srslte/common/test_common.h"
#include "srslte/phy/fec/polar/polar_interleaver.h"

int main(int argc, char** argv)
{
  uint32_t idx = 0;
  while (polar_interleaver_gold[idx].K) {
    uint32_t K = polar_interleaver_gold[idx].K;

    // Create indexes in order
    uint16_t indexes_in[SRSLTE_POLAR_INTERLEAVER_K_MAX_IL];
    for (uint16_t i = 0; i < (uint16_t)K; i++) {
      indexes_in[i] = i;
    }

    // Run interleaver forward
    uint16_t indexes_out[SRSLTE_POLAR_INTERLEAVER_K_MAX_IL];
    srslte_polar_interleaver_run_u16(indexes_in, indexes_out, K, true);

    // Check indexes
    for (uint32_t i = 0; i < K; i++) {
      TESTASSERT(polar_interleaver_gold[idx].indexes[i] == indexes_out[i]);
    }

    // Run interleaver backwards
    srslte_polar_interleaver_run_u16(indexes_out, indexes_in, K, false);

    // Check indexes
    for (uint16_t i = 0; i < (uint16_t)K; i++) {
      TESTASSERT(indexes_in[i] == i);
    }

    idx++;
  }

  return SRSLTE_SUCCESS;
}