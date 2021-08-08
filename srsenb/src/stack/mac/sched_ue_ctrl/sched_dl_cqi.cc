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

#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_dl_cqi.h"
#include "srsenb/hdr/stack/mac/schedulers/sched_base.h"

namespace srsenb {

rbgmask_t sched_dl_cqi::get_optim_rbgmask(const rbgmask_t& dl_mask, uint32_t req_rbgs, bool max_flag) const
{
  req_rbgs = std::min(req_rbgs, cell_nof_rbg);
  if (not subband_cqi_enabled()) {
    // in case of wideband, just find any available RBGs
    return find_available_rbgmask(req_rbgs, false, dl_mask);
  }

  rbgmask_t emptymask = ~dl_mask;
  if (emptymask.none() or req_rbgs >= emptymask.size() or emptymask.count() <= req_rbgs) {
    return emptymask;
  }

  srsran::bounded_vector<uint32_t, MAX_NOF_RBGS> sorted_cqi_pos;
  srsran::bounded_vector<uint32_t, MAX_NOF_RBGS> sorted_cqis;
  for (int pos = emptymask.find_lowest(0, emptymask.size(), true); pos >= 0;
       pos     = emptymask.find_lowest(pos + 1, emptymask.size(), true)) {
    sorted_cqis.push_back(get_rbg_cqi(pos));
    sorted_cqi_pos.push_back(pos);
  }
  std::stable_sort(sorted_cqi_pos.begin(), sorted_cqi_pos.end(), [&sorted_cqis](uint32_t lhs, uint32_t rhs) {
    return sorted_cqis[lhs] > sorted_cqis[rhs];
  });
  if (max_flag) {
    for (size_t i = req_rbgs; i < sorted_cqi_pos.size(); ++i) {
      emptymask.set(sorted_cqi_pos[i], false);
    }
  } else {
    for (size_t i = 0; i < sorted_cqi_pos.size() - req_rbgs; ++i) {
      emptymask.set(sorted_cqi_pos[i], false);
    }
  }

  return emptymask;
}

rbgmask_t find_min_cqi_rbgs(const rbgmask_t& mask, const sched_dl_cqi& dl_cqi, int& min_cqi)
{
  if (mask.none()) {
    min_cqi = -1;
    return mask;
  }

  if (not dl_cqi.subband_cqi_enabled()) {
    min_cqi = dl_cqi.get_wb_cqi_info();
    return mask;
  }

  rbgmask_t min_mask(mask.size());
  int       rbg = mask.find_lowest(0, mask.size());
  min_cqi       = std::numeric_limits<int>::max();
  for (; rbg != -1; rbg = mask.find_lowest(rbg, mask.size())) {
    uint32_t sb  = dl_cqi.rbg_to_sb_index(rbg);
    int      cqi = dl_cqi.get_subband_cqi(sb);
    if (cqi < min_cqi) {
      min_cqi = cqi;
      min_mask.reset();
      min_mask.set(rbg);
    } else if (cqi == min_cqi) {
      min_mask.set(rbg);
    }
    rbg++;
  }
  min_cqi = min_cqi == std::numeric_limits<int>::max() ? -1 : min_cqi;

  return min_mask;
}

rbgmask_t remove_min_cqi_rbgs(const rbgmask_t& rbgmask, const sched_dl_cqi& dl_cqi)
{
  int       min_cqi;
  rbgmask_t minmask = find_min_cqi_rbgs(rbgmask, dl_cqi, min_cqi);
  if (min_cqi < 0) {
    return minmask;
  }
  minmask = ~minmask & rbgmask;
  return minmask;
}

} // namespace srsenb