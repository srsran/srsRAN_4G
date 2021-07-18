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

#include "srsenb/hdr/stack/mac/nr/sched_nr_rb.h"

namespace srsenb {
namespace sched_nr_impl {

/// TS 38.214, Table 6.1.2.2.1-1 - Nominal RBG size P
uint32_t get_P(uint32_t bwp_nof_prb, bool config_1_or_2)
{
  srsran_assert(bwp_nof_prb > 0 and bwp_nof_prb <= 275, "Invalid BWP size");
  if (bwp_nof_prb <= 36) {
    return config_1_or_2 ? 2 : 4;
  }
  if (bwp_nof_prb <= 72) {
    return config_1_or_2 ? 4 : 8;
  }
  if (bwp_nof_prb <= 144) {
    return config_1_or_2 ? 8 : 16;
  }
  return 16;
}

/// TS 38.214 - total number of RBGs for a uplink bandwidth part of size "bwp_nof_prb" PRBs
uint32_t get_nof_rbgs(uint32_t bwp_nof_prb, uint32_t bwp_start, bool config1_or_2)
{
  uint32_t P = get_P(bwp_nof_prb, config1_or_2);
  return srsran::ceil_div(bwp_nof_prb + (bwp_start % P), P);
}

uint32_t get_rbg_size(uint32_t bwp_nof_prb, uint32_t bwp_start, bool config1_or_2, uint32_t rbg_idx)
{
  uint32_t P        = get_P(bwp_nof_prb, config1_or_2);
  uint32_t nof_rbgs = get_nof_rbgs(bwp_nof_prb, bwp_start, config1_or_2);
  if (rbg_idx == 0) {
    return P - (bwp_start % P);
  }
  if (rbg_idx == nof_rbgs - 1) {
    uint32_t ret = (bwp_start + bwp_nof_prb) % P;
    return ret > 0 ? ret : P;
  }
  return P;
}

bwp_rb_bitmap::bwp_rb_bitmap(uint32_t bwp_nof_prbs, uint32_t bwp_prb_start_, bool config1_or_2) :
  prbs_(bwp_nof_prbs),
  rbgs_(get_nof_rbgs(bwp_nof_prbs, bwp_prb_start_, config1_or_2)),
  P_(get_P(bwp_nof_prbs, config1_or_2)),
  Pnofbits(log2(P_)),
  first_rbg_size(get_rbg_size(bwp_nof_prbs, bwp_prb_start_, config1_or_2, 0))
{}

uint32_t bwp_rb_bitmap::prb_to_rbg_idx(uint32_t prb_idx) const
{
  return ((prb_idx + P_ - first_rbg_size) >> Pnofbits);
}

void bwp_rb_bitmap::add_prbs_to_rbgs(const prb_bitmap& grant)
{
  int idx = 0;
  do {
    idx = grant.find_lowest(idx, grant.size(), true);
    if (idx < 0) {
      return;
    }
    uint32_t rbg_idx = prb_to_rbg_idx(idx);
    rbgs_.set(rbg_idx, true);
    idx++;
  } while (idx != (int)prbs_.size());
}

void bwp_rb_bitmap::add_prbs_to_rbgs(const prb_interval& grant)
{
  uint32_t rbg_start = prb_to_rbg_idx(grant.start());
  uint32_t rbg_stop  = std::min(prb_to_rbg_idx(grant.stop() - 1) + 1u, (uint32_t)rbgs_.size());
  rbgs_.fill(rbg_start, rbg_stop);
}

void bwp_rb_bitmap::add_rbgs_to_prbs(const rbg_bitmap& grant)
{
  int idx = 0;
  do {
    idx = grant.find_lowest(idx, grant.size(), true);
    if (idx < 0) {
      return;
    }
    uint32_t prb_idx = (idx - 1) * P_ + first_rbg_size;
    uint32_t prb_end = std::min(prb_idx + ((idx == 0) ? first_rbg_size : P_), (uint32_t)prbs_.size());
    prbs_.fill(prb_idx, prb_end);
    idx++;
  } while (idx != (int)prbs_.size());
}

} // namespace sched_nr_impl
} // namespace srsenb
