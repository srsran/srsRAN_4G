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

#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_phy_resource.h"
extern "C" {
#include "lib/include/srsran/phy/dft/dft_precoding.h"
}

namespace srsenb {

rbg_interval rbg_interval::find_first_interval(const rbgmask_t& mask)
{
  int rb_start = mask.find_lowest(0, mask.size());
  if (rb_start != -1) {
    int rb_end = mask.find_lowest(rb_start + 1, mask.size(), false);
    return rbg_interval(rb_start, rb_end < 0 ? mask.size() : rb_end);
  }
  return rbg_interval();
}

prb_interval prb_interval::riv_to_prbs(uint32_t riv, uint32_t nof_prbs, int nof_vrbs)
{
  if (nof_vrbs < 0) {
    nof_vrbs = nof_prbs;
  }
  uint32_t rb_start, l_crb;
  srsran_ra_type2_from_riv(riv, &l_crb, &rb_start, nof_prbs, (uint32_t)nof_vrbs);
  return {rb_start, rb_start + l_crb};
}

template <typename RBMask,
          typename RBInterval =
              typename std::conditional<std::is_same<RBMask, prbmask_t>::value, prb_interval, rbg_interval>::type>
RBInterval find_contiguous_interval(const RBMask& in_mask, uint32_t max_size)
{
  RBInterval max_interv;

  for (size_t n = 0; n < in_mask.size();) {
    int pos = in_mask.find_lowest(n, in_mask.size(), false);
    if (pos < 0) {
      break;
    }

    size_t     max_pos = std::min(in_mask.size(), (size_t)pos + max_size);
    int        pos2    = in_mask.find_lowest(pos + 1, max_pos, true);
    RBInterval interv(pos, pos2 < 0 ? max_pos : pos2);
    if (interv.length() >= max_size) {
      return interv;
    }
    if (interv.length() > max_interv.length()) {
      max_interv = interv;
    }
    n = interv.stop();
  }
  return max_interv;
}

rbgmask_t find_available_rbgmask(const rbgmask_t& in_mask, uint32_t max_size)
{
  // 1's for free RBs
  rbgmask_t localmask = ~(in_mask);

  if (max_size >= localmask.size() or max_size >= localmask.count()) {
    // shortcut in case rbg count < max_size
    return localmask;
  }

  uint32_t i = 0, nof_alloc = 0;
  for (; i < localmask.size() and nof_alloc < max_size; ++i) {
    if (localmask.test(i)) {
      nof_alloc++;
    }
  }
  localmask.fill(i, localmask.size(), false);
  return localmask;
}

rbg_interval find_empty_rbg_interval(uint32_t max_nof_rbgs, const rbgmask_t& current_mask)
{
  return find_contiguous_interval(current_mask, max_nof_rbgs);
}

rbgmask_t find_available_rbgmask(uint32_t max_nof_rbgs, bool is_contiguous, const rbgmask_t& current_mask)
{
  // Allocate enough RBs that accommodate pending data
  rbgmask_t newtx_mask(current_mask.size());
  if (is_contiguous) {
    rbg_interval interv = find_contiguous_interval(current_mask, max_nof_rbgs);
    newtx_mask.fill(interv.start(), interv.stop());
  } else {
    newtx_mask = find_available_rbgmask(current_mask, max_nof_rbgs);
  }
  return newtx_mask;
}

prb_interval find_contiguous_ul_prbs(uint32_t L, const prbmask_t& current_mask)
{
  prb_interval prb_interv = find_contiguous_interval(current_mask, L);
  if (prb_interv.empty()) {
    return prb_interv;
  }

  // Make sure L is allowed by SC-FDMA modulation
  prb_interval prb_interv2 = prb_interv;
  while (not srsran_dft_precoding_valid_prb(prb_interv.length()) and prb_interv.stop() < current_mask.size() and
         not current_mask.test(prb_interv.stop())) {
    prb_interv.resize_by(1);
  }
  if (not srsran_dft_precoding_valid_prb(prb_interv.length())) {
    // if length increase failed, try to decrease
    prb_interv = prb_interv2;
    prb_interv.resize_by(-1);
    while (not srsran_dft_precoding_valid_prb(prb_interv.length()) and not prb_interv.empty()) {
      prb_interv.resize_by(-1);
    }
  }
  return prb_interv;
}

} // namespace srsenb