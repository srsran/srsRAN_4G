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

#include "srslte/phy/ch_estimation/dmrs_pdcch.h"

uint32_t srslte_pdcch_calculate_Y_p_n(uint32_t coreset_id, uint16_t rnti, int n)
{
  const uint32_t A_p[3] = {39827, 39829, 39839};
  const uint32_t D      = 65537;

  if (n < 0) {
    return rnti;
  }

  return (A_p[coreset_id % 3] * srslte_pdcch_calculate_Y_p_n(coreset_id, rnti, n - 1)) % D;
}

/**
 * Calculates the Control Channnel Element As described in 3GPP 38.213 R15 10.1 UE procedure for determining physical
 * downlink control channel assignment
 *
 */
int srslte_pdcch_get_ncce(const srslte_nr_pdcch_cfg_t* cfg, const srslte_dl_sf_cfg_t* dl_sf)
{
  if (cfg->aggregation_level >= SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS) {
    ERROR("Invalid aggregation level %d;\n", cfg->aggregation_level);
    return SRSLTE_ERROR;
  }

  uint32_t L    = 1U << cfg->aggregation_level;                             // Aggregation level
  uint32_t n_ci = 0;                                                        //  Carrier indicator field
  uint32_t m    = cfg->candidate;                                           // Selected PDDCH candidate
  uint32_t M    = cfg->search_space.nof_candidates[cfg->aggregation_level]; // Number of aggregation levels

  if (M == 0) {
    ERROR("Invalid number of candidates %d for aggregation level %d\n", M, cfg->aggregation_level);
    return SRSLTE_ERROR;
  }

  // Count number of REG
  uint32_t N_cce = 0;
  for (uint32_t i = 0; i < SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    N_cce += cfg->coreset.freq_domain_resources[i] ? cfg->coreset.duration : 0;
  }

  if (N_cce < L) {
    ERROR("Error number of CCE %d is lower than the aggregation level %d\n", N_cce, L);
    return SRSLTE_ERROR;
  }

  // Calculate Y_p_n
  uint32_t Y_p_n = 0;
  if (cfg->search_space.type == srslte_search_space_type_ue) {
    Y_p_n = srslte_pdcch_calculate_Y_p_n(
        cfg->coreset.id, cfg->rnti, dl_sf->tti % SRSLTE_NR_NSLOTS_PER_FRAME(cfg->carrier.numerology));
  }

  return (int)(L * ((Y_p_n + (m * N_cce) / (L * M) + n_ci) % (N_cce / L)));
}

static uint32_t dmrs_pdcch_get_cinit(uint32_t slot_idx, uint32_t symbol_idx, uint32_t n_id)
{
  return (uint32_t)((((SRSLTE_NR_NSYMB_PER_SLOT * slot_idx + symbol_idx + 1UL) << 17UL) * (2 * n_id + 1) + 2 * n_id) &
                    (uint64_t)INT32_MAX);
}

static void
dmrs_pdcch_put_symbol_noninterleaved(const srslte_nr_pdcch_cfg_t* cfg, uint32_t cinit, uint32_t ncce, cf_t* sf_symbol)
{
  uint32_t L            = 1U << cfg->aggregation_level;
  uint32_t nof_freq_res = SRSLTE_MIN(cfg->carrier.nof_prb / 6, SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE);

  // Initialise sequence for this symbol
  srslte_sequence_state_t sequence_state = {};
  srslte_sequence_state_init(&sequence_state, cinit);
  uint32_t sequence_skip = 0; // Accumulates pilot locations to skip

  // Calculate Resource block indexes range, every CCE is 6 REG, 1 REG is 6 RE in resource blocks
  uint32_t rb_coreset_idx_begin = (ncce * 6) / cfg->coreset.duration;
  uint32_t rb_coreset_idx_end   = ((ncce + L) * 6) / cfg->coreset.duration;

  // CORESET Resource Block counter
  uint32_t rb_coreset_idx = 0;
  for (uint32_t i = 0; i < nof_freq_res; i++) {
    // Every frequency resource is 6 Resource blocks, every resource block carries 3 pilots. So 18 possible pilots per
    // frequency resource.
    const uint32_t nof_pilots_x_resource = 18;

    // Skip frequency resource if outside of the CORESET
    if (!cfg->coreset.freq_domain_resources[i]) {
      // Skip possible DMRS locations in this region
      sequence_skip += nof_pilots_x_resource;
      continue;
    }

    // Skip if the frequency resource highest RB is lower than the first CCE resource block.
    if ((rb_coreset_idx + 6) <= rb_coreset_idx_begin) {
      // Skip possible DMRS locations in this region
      sequence_skip += nof_pilots_x_resource;

      // Since this is part of the CORESET, count the RB as CORESET
      rb_coreset_idx += 6;
      continue;
    }

    // Return if the first RB of the frequency resource is greater than the last CCE resource block
    if (rb_coreset_idx > rb_coreset_idx_end) {
      return;
    }

    // Skip all discarded possible pilot locations
    srslte_sequence_state_advance(&sequence_state, 2 * sequence_skip);
    sequence_skip = 0;

    // Generate pilots
    cf_t rl[nof_pilots_x_resource];
    srslte_sequence_state_gen_f(&sequence_state, M_SQRT1_2, (float*)rl, nof_pilots_x_resource * 2);

    // For each RB in the frequency resource
    for (uint32_t j = 0; j < 6; j++) {
      // Calculate absolute RB index
      uint32_t n = i * 6 + j;

      // Skip if lower than begin
      if (rb_coreset_idx < rb_coreset_idx_begin) {
        rb_coreset_idx++;
        continue;
      }

      // Return if greater than end
      if (rb_coreset_idx >= rb_coreset_idx_end) {
        return;
      }

      // Write pilots in the symbol
      for (uint32_t k_prime = 0; k_prime < 3; k_prime++) {
        // Calculate sub-carrier index
        uint32_t k = n * SRSLTE_NRE + 4 * k_prime + 1;

        sf_symbol[k] = rl[(3 * n + k_prime) % nof_pilots_x_resource];
      }
      rb_coreset_idx++;
    }
  }
}
