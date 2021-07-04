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

#include "srsenb/hdr/stack/mac/nr/sched_nr_common.h"

namespace srsenb {
namespace sched_nr_impl {

sched_cell_params::sched_cell_params(uint32_t cc_, const cell_cfg_t& cell, const sched_cfg_t& sched_cfg_) :
  cc(cc_), cell_cfg(cell), sched_cfg(sched_cfg_)
{}

sched_params::sched_params(const sched_cfg_t& sched_cfg_) : sched_cfg(sched_cfg_) {}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void get_dci_locs(const srsran_coreset_t&      coreset,
                  const srsran_search_space_t& search_space,
                  uint16_t                     rnti,
                  bwp_cce_pos_list&            cce_locs)
{
  for (uint32_t sl = 0; sl < SRSRAN_NOF_SF_X_FRAME; ++sl) {
    for (uint32_t agg_idx = 0; agg_idx < MAX_NOF_AGGR_LEVELS; ++agg_idx) {
      pdcch_cce_pos_list pdcch_locs;
      cce_locs[sl][agg_idx].resize(pdcch_locs.capacity());
      uint32_t n =
          srsran_pdcch_nr_locations_coreset(&coreset, &search_space, rnti, agg_idx, sl, cce_locs[sl][agg_idx].data());
      cce_locs[sl][agg_idx].resize(n);
    }
  }
}

} // namespace sched_nr_impl
} // namespace srsenb