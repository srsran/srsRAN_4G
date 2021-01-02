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