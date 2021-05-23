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

std::tuple<uint32_t, int> find_min_cqi_rbg(const rbgmask_t& mask, const sched_dl_cqi& dl_cqi)
{
  if (mask.none()) {
    return std::make_tuple(mask.size(), -1);
  }

  int rbg = mask.find_lowest(0, mask.size());
  if (not dl_cqi.subband_cqi_enabled()) {
    return std::make_tuple(rbg, dl_cqi.get_wb_cqi_info());
  }

  int      min_cqi = std::numeric_limits<int>::max();
  uint32_t min_rbg = mask.size();
  for (; rbg != -1; rbg = mask.find_lowest(rbg, mask.size())) {
    uint32_t sb  = dl_cqi.rbg_to_sb_index(rbg);
    int      cqi = dl_cqi.get_subband_cqi(sb);
    if (cqi < min_cqi) {
      min_cqi = cqi;
      min_rbg = rbg;
    }
    rbg = (int)srsran::ceil_div((sb + 1U) * mask.size(), dl_cqi.nof_subbands()); // skip to next subband index
  }
  return min_cqi != std::numeric_limits<int>::max() ? std::make_tuple(min_rbg, min_cqi) : std::make_tuple(0u, -1);
}

rbgmask_t remove_min_cqi_subband(const rbgmask_t& rbgmask, const sched_dl_cqi& dl_cqi)
{
  std::tuple<uint32_t, int> tup = find_min_cqi_rbg(rbgmask, dl_cqi);
  if (std::get<1>(tup) < 0) {
    return rbgmask_t(rbgmask.size());
  }
  uint32_t sb        = dl_cqi.rbg_to_sb_index(std::get<0>(tup));
  uint32_t rbg_begin = sb * rbgmask.size() / dl_cqi.nof_subbands();
  uint32_t rbg_end   = srsran::ceil_div((sb + 1) * rbgmask.size(), dl_cqi.nof_subbands());

  rbgmask_t ret(rbgmask);
  ret.fill(rbg_begin, rbg_end, false);
  return ret;
}

rbgmask_t remove_min_cqi_rbg(const rbgmask_t& rbgmask, const sched_dl_cqi& dl_cqi)
{
  std::tuple<uint32_t, int> tup = find_min_cqi_rbg(rbgmask, dl_cqi);
  if (std::get<1>(tup) < 0) {
    return rbgmask_t(rbgmask.size());
  }
  rbgmask_t ret(rbgmask);
  ret.set(std::get<0>(tup), false);
  return ret;
}

} // namespace srsenb