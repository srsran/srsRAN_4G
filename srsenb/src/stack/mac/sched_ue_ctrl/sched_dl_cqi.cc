/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_dl_cqi.h"
#include "srsenb/hdr/stack/mac/schedulers/sched_base.h"

using namespace srsenb;

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
