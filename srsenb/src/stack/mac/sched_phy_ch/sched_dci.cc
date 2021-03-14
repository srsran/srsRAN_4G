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

#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_dci.h"
#include "srsenb/hdr/stack/mac/sched_common.h"
#include <cmath>
#include <cstdint>

namespace srsenb {

/// Compute max TBS based on max coderate
int coderate_to_tbs(float max_coderate, uint32_t nof_re)
{
  return static_cast<int>(floorf(nof_re * max_coderate - 24));
}

/// Compute {mcs, tbs_idx} based on {max_tbs, nof_prb}
int compute_mcs_from_max_tbs(uint32_t nof_prb,
                             uint32_t max_tbs,
                             uint32_t max_mcs,
                             bool     is_ul,
                             bool     use_tbs_index_alt,
                             int&     mcs,
                             int&     tbs_idx)
{
  constexpr static std::array<int, 6> forbidden_tbs_idx_alt{1, 3, 5, 7, 9, 26};

  // Compute I_TBS based on max TBS
  uint32_t max_tbs_idx = (use_tbs_index_alt) ? 33 : 26;
  tbs_idx              = srslte_ra_tbs_to_table_idx(max_tbs, nof_prb, max_tbs_idx);
  if (tbs_idx <= 0) {
    return SRSLTE_ERROR;
  }
  --tbs_idx; // get TBS index lower bound
  if (use_tbs_index_alt and
      std::find(forbidden_tbs_idx_alt.begin(), forbidden_tbs_idx_alt.end(), tbs_idx) != forbidden_tbs_idx_alt.end()) {
    // some tbs_idx are invalid for 256QAM. See TS 36.213 - Table 7.1.7.1-1A
    --tbs_idx;
  }

  // Compute I_mcs based on I_TBS. Reverse of TS 36.213 - Table 7.1.7.1-1/1A
  mcs = srslte_ra_mcs_from_tbs_idx(tbs_idx, use_tbs_index_alt, is_ul);
  if (mcs < 0) {
    return SRSLTE_ERROR;
  }
  if (mcs > (int)max_mcs) {
    // bound mcs
    mcs     = max_mcs;
    tbs_idx = srslte_ra_tbs_idx_from_mcs(mcs, use_tbs_index_alt, is_ul);
  }

  return SRSLTE_SUCCESS;
}

tbs_info compute_mcs_and_tbs(uint32_t nof_prb,
                             uint32_t nof_re,
                             uint32_t cqi,
                             uint32_t max_mcs,
                             bool     is_ul,
                             bool     ulqam64_enabled,
                             bool     use_tbs_index_alt)
{
  assert((not is_ul or not use_tbs_index_alt) && "UL cannot use Alt CQI Table");
  assert((is_ul or not ulqam64_enabled) && "DL cannot use UL-QAM64 enable flag");

  float    max_coderate = srslte_cqi_to_coderate(std::min(cqi + 1U, 15U), use_tbs_index_alt);
  uint32_t max_Qm       = (is_ul) ? (ulqam64_enabled ? 6 : 4) : (use_tbs_index_alt ? 8 : 6);
  max_coderate          = std::min(max_coderate, 0.93F * max_Qm);

  int   mcs               = 0;
  float prev_max_coderate = 0;
  do {
    // update max TBS based on max coderate
    int max_tbs = coderate_to_tbs(max_coderate, nof_re);
    if (max_tbs < 16) {
      return tbs_info{};
    }

    // Compute max {MCS,I_TBS} based on given max_tbs, nof_prb
    int tbs_idx = 0;
    if (compute_mcs_from_max_tbs(nof_prb, max_tbs, max_mcs, is_ul, use_tbs_index_alt, mcs, tbs_idx) != SRSLTE_SUCCESS) {
      return tbs_info{};
    }

    if (mcs == 6 and nof_prb == 1) {
      // Avoid the unusual case n_prb=1, mcs=6 tbs=328 (used in voip)
      max_mcs = mcs - 1;
      continue;
    }

    // compute real TBS and coderate based on maximum achievable MCS
    int   tbs      = srslte_ra_tbs_from_idx(tbs_idx, nof_prb);
    float coderate = srslte_coderate(tbs, nof_re);

    // update max coderate based on mcs
    srslte_mod_t mod = (is_ul) ? srslte_ra_ul_mod_from_mcs(mcs) : srslte_ra_dl_mod_from_mcs(mcs, use_tbs_index_alt);
    uint32_t     Qm  = srslte_mod_bits_x_symbol(mod);
    max_coderate     = std::min(0.93F * Qm, max_coderate);

    if (coderate <= max_coderate) {
      // solution was found
      tbs_info tb;
      tb.tbs_bytes = tbs / 8;
      tb.mcs       = mcs;
      return tb;
    }

    // start with smaller max mcs in next iteration
    max_mcs = mcs - 1;
  } while (mcs > 0 and max_coderate != prev_max_coderate);

  return tbs_info{};
}

tbs_info compute_min_mcs_and_tbs_from_required_bytes(uint32_t nof_prb,
                                                     uint32_t nof_re,
                                                     uint32_t cqi,
                                                     uint32_t max_mcs,
                                                     uint32_t req_bytes,
                                                     bool     is_ul,
                                                     bool     ulqam64_enabled,
                                                     bool     use_tbs_index_alt)
{
  // get max MCS/TBS that meets max coderate requirements
  tbs_info tb_max = compute_mcs_and_tbs(nof_prb, nof_re, cqi, max_mcs, is_ul, ulqam64_enabled, use_tbs_index_alt);
  if (tb_max.tbs_bytes + 8 <= (int)req_bytes or tb_max.mcs == 0 or req_bytes <= 0) {
    // if mcs cannot be lowered or a decrease in TBS index won't meet req_bytes requirement
    return tb_max;
  }

  // get maximum MCS that leads to tbs < req_bytes (used as max_tbs argument)
  int mcs_min     = 0;
  int tbs_idx_min = 0;
  // Note: we subtract -1 to required data to get an exclusive lower bound for maximum MCS. This works ok because
  //       req_bytes * 8 is always even
  if (compute_mcs_from_max_tbs(nof_prb, req_bytes * 8U - 1, max_mcs, is_ul, use_tbs_index_alt, mcs_min, tbs_idx_min) !=
      SRSLTE_SUCCESS) {
    // Failed to compute maximum MCS that leads to TBS < req bytes. MCS=0 is likely a valid solution
    tbs_info tb2 = compute_mcs_and_tbs(nof_prb, nof_re, cqi, 0, is_ul, ulqam64_enabled, use_tbs_index_alt);
    if (tb2.tbs_bytes >= (int)req_bytes) {
      return tb2;
    }
    return tb_max;
  }

  // Iterate from min to max MCS until a solution is found
  for (int mcs = mcs_min + 1; mcs < tb_max.mcs; ++mcs) {
    tbs_info tb2 = compute_mcs_and_tbs(nof_prb, nof_re, cqi, mcs, is_ul, ulqam64_enabled, use_tbs_index_alt);
    if (tb2.tbs_bytes >= (int)req_bytes) {
      return tb2;
    }
  }
  return tb_max;
}

} // namespace srsenb
