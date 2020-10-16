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

#include "srslte/phy/phch/pdcch_nr.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

/**
 * @brief Recursive Y_p_n function
 */
static uint32_t srslte_pdcch_calculate_Y_p_n(uint32_t coreset_id, uint16_t rnti, int n)
{
  static const uint32_t A_p[3] = {39827, 39829, 39839};
  const uint32_t        D      = 65537;

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
static int srslte_pdcch_nr_get_ncce(const srslte_coreset_t*      coreset,
                                    const srslte_search_space_t* search_space,
                                    uint16_t                     rnti,
                                    uint32_t                     aggregation_level,
                                    uint32_t                     slot_idx,
                                    uint32_t                     candidate)
{
  if (aggregation_level >= SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS) {
    ERROR("Invalid aggregation level %d;\n", aggregation_level);
    return SRSLTE_ERROR;
  }

  uint32_t L    = 1U << aggregation_level;                         // Aggregation level
  uint32_t n_ci = 0;                                               //  Carrier indicator field
  uint32_t m    = candidate;                                       // Selected PDDCH candidate
  uint32_t M    = search_space->nof_candidates[aggregation_level]; // Number of aggregation levels

  if (M == 0) {
    ERROR("Invalid number of candidates %d for aggregation level %d\n", M, aggregation_level);
    return SRSLTE_ERROR;
  }

  // Every REG is 1PRB wide and a CCE is 6 REG. So, the number of N_CCE is a sixth of the bandwidth times the number of
  // symbols
  uint32_t N_cce = srslte_coreset_get_bw(coreset) * coreset->duration / 6;

  if (N_cce < L) {
    ERROR("Error number of CCE %d is lower than the aggregation level %d\n", N_cce, L);
    return SRSLTE_ERROR;
  }

  // Calculate Y_p_n for UE search space only
  uint32_t Y_p_n = 0;
  if (search_space->type == srslte_search_space_type_ue) {
    Y_p_n = srslte_pdcch_calculate_Y_p_n(coreset->id, rnti, slot_idx);
  }

  return (int)(L * ((Y_p_n + (m * N_cce) / (L * M) + n_ci) % (N_cce / L)));
}

int srslte_pdcch_nr_locations_ncce(const srslte_coreset_t*      coreset,
                                   const srslte_search_space_t* search_space,
                                   uint16_t                     rnti,
                                   uint32_t                     aggregation_level,
                                   uint32_t                     slot_idx,
                                   uint32_t                     locations[SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES])
{
  if (coreset == NULL || search_space == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  uint32_t nof_candidates = search_space->nof_candidates[aggregation_level];

  nof_candidates = SRSLTE_MIN(nof_candidates, SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES);

  for (uint32_t candidate = 0; candidate < nof_candidates; candidate++) {
    int ret = srslte_pdcch_nr_get_ncce(coreset, search_space, rnti, aggregation_level, slot_idx, candidate);
    if (ret < SRSLTE_SUCCESS) {
      return ret;
    }

    locations[candidate] = ret;
  }

  return nof_candidates;
}
