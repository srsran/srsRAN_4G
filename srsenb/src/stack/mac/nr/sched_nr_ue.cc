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

#include "srsenb/hdr/stack/mac/nr/sched_nr_ue.h"

namespace srsenb {
namespace sched_nr_impl {

bwp_ue::bwp_ue(ue_carrier& carrier_, tti_point tti_rx_) : carrier(&carrier_), tti_rx(tti_rx_), cc(carrier_.cc) {}

bwp_ue::~bwp_ue()
{
  if (carrier != nullptr) {
    carrier->release();
  }
}

ue_carrier::ue_carrier(uint16_t rnti_, uint32_t cc_, const sched_nr_ue_cfg& cfg_) : rnti(rnti_), cc(cc_), cfg(&cfg_) {}

void ue_carrier::push_feedback(srsran::move_callback<void(ue_carrier&)> callback)
{
  pending_feedback.push_back(std::move(callback));
}

bwp_ue ue_carrier::try_reserve(tti_point tti_rx)
{
  if (busy) {
    return bwp_ue();
  }
  // successfully acquired
  busy = true;
  while (not pending_feedback.empty()) {
    pending_feedback.front()(*this);
    pending_feedback.pop_front();
  }
  return bwp_ue(*this, tti_rx);
}

ue::ue(uint16_t rnti, const sched_nr_ue_cfg& cfg)
{
  for (uint32_t cc = 0; cc < cfg.carriers.size(); ++cc) {
    if (cfg.carriers[cc].active) {
      carriers[cc].reset(new ue_carrier(rnti, cc, cfg));
    }
  }
}

void ue::set_cfg(const sched_nr_ue_cfg& cfg)
{
  current_idx          = (current_idx + 1) % ue_cfgs.size();
  ue_cfgs[current_idx] = cfg;
}

bwp_ue ue::try_reserve(tti_point tti_rx, uint32_t cc)
{
  if (carriers[cc] == nullptr) {
    return bwp_ue();
  }
  bwp_ue sfu = carriers[cc]->try_reserve(tti_rx);
  if (sfu.empty()) {
    return bwp_ue();
  }
  // set UE-common parameters
  sfu.pending_sr = pending_sr;
  sfu.cfg        = &ue_cfgs[current_idx];

  return sfu;
}

} // namespace sched_nr_impl
} // namespace srsenb
