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
#include "srsenb/hdr/stack/mac/nr/sched_nr_pdcch.h"

namespace srsenb {
namespace sched_nr_impl {

ue_cfg_extended::ue_cfg_extended(uint16_t rnti_, const ue_cfg_t& uecfg) : ue_cfg_t(uecfg), rnti(rnti_)
{
  cc_params.resize(carriers.size());
  for (uint32_t cc = 0; cc < cc_params.size(); ++cc) {
    cc_params[cc].bwps.resize(1);
    auto& bwp = cc_params[cc].bwps[0];
    for (uint32_t ssid = 0; ssid < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++ssid) {
      if (phy_cfg.pdcch.search_space_present[ssid]) {
        bwp.search_spaces.emplace_back();
        bwp.search_spaces.back().cfg = &phy_cfg.pdcch.search_space[ssid];
      }
    }
    for (uint32_t csid = 0; csid < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; ++csid) {
      if (phy_cfg.pdcch.coreset_present[csid]) {
        bwp.coresets.emplace_back();
        auto& coreset = bwp.coresets.back();
        coreset.cfg   = &phy_cfg.pdcch.coreset[csid];
        for (auto& ss : bwp.search_spaces) {
          if (ss.cfg->coreset_id == csid + 1) {
            coreset.ss_list.push_back(&ss);
            get_dci_locs(*coreset.cfg, *coreset.ss_list.back()->cfg, rnti, coreset.cce_positions);
          }
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

slot_ue::slot_ue(resource_guard::token ue_token_, uint16_t rnti_, tti_point tti_rx_, uint32_t cc_) :
  ue_token(std::move(ue_token_)), rnti(rnti_), tti_rx(tti_rx_), cc(cc_)
{}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_carrier::ue_carrier(uint16_t rnti_, uint32_t cc_, const ue_cfg_t& uecfg_) : rnti(rnti_), cc(cc_), cfg(&uecfg_) {}

void ue_carrier::push_feedback(srsran::move_callback<void(ue_carrier&)> callback)
{
  pending_feedback.push_back(std::move(callback));
}

slot_ue ue_carrier::try_reserve(tti_point tti_rx, const ue_cfg_extended& uecfg_)
{
  slot_ue sfu(busy, rnti, tti_rx, cc);
  if (sfu.empty()) {
    return sfu;
  }
  // successfully acquired. Process any CC-specific pending feedback
  cfg = &uecfg_;
  while (not pending_feedback.empty()) {
    pending_feedback.front()(*this);
    pending_feedback.pop_front();
  }
  if (not last_tti_rx.is_valid()) {
    last_tti_rx = tti_rx;
    harq_ent.new_tti(tti_rx);
  } else {
    while (last_tti_rx++ < tti_rx) {
      harq_ent.new_tti(tti_rx);
    }
  }

  // set UE parameters common to all carriers
  sfu.cfg = &uecfg_;

  // copy cc-specific parameters and find available HARQs
  sfu.cc_cfg    = &uecfg_.carriers[cc];
  sfu.pdcch_tti = tti_rx + TX_ENB_DELAY;
  sfu.pdsch_tti = sfu.pdcch_tti + sfu.cc_cfg->pdsch_res_list[0].k0;
  sfu.pusch_tti = sfu.pdcch_tti + sfu.cc_cfg->pusch_res_list[0].k2;
  sfu.uci_tti   = sfu.pdsch_tti + sfu.cc_cfg->pdsch_res_list[0].k1;
  sfu.dl_cqi    = dl_cqi;
  sfu.ul_cqi    = ul_cqi;
  sfu.h_dl      = harq_ent.find_pending_dl_retx();
  if (sfu.h_dl == nullptr) {
    sfu.h_dl = harq_ent.find_empty_dl_harq();
  }
  sfu.h_ul = harq_ent.find_pending_ul_retx();
  if (sfu.h_ul == nullptr) {
    sfu.h_ul = harq_ent.find_empty_ul_harq();
  }

  if (sfu.h_dl == nullptr and sfu.h_ul == nullptr) {
    // there needs to be at least one available HARQ for newtx/retx
    sfu.release();
    return sfu;
  }
  return sfu;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue::ue(uint16_t rnti_, const ue_cfg_t& cfg) : rnti(rnti_)
{
  ue_cfgs[0] = ue_cfg_extended(rnti, cfg);
  for (uint32_t cc = 0; cc < cfg.carriers.size(); ++cc) {
    if (cfg.carriers[cc].active) {
      carriers[cc].reset(new ue_carrier(rnti, cc, cfg));
    }
  }
}

void ue::set_cfg(const ue_cfg_t& cfg)
{
  current_idx          = (current_idx + 1U) % ue_cfgs.size();
  ue_cfgs[current_idx] = ue_cfg_extended(rnti, cfg);
}

slot_ue ue::try_reserve(tti_point tti_rx, uint32_t cc)
{
  if (carriers[cc] == nullptr) {
    return slot_ue();
  }
  slot_ue sfu = carriers[cc]->try_reserve(tti_rx, ue_cfgs[current_idx]);
  if (sfu.empty()) {
    return slot_ue();
  }
  // set UE-common parameters
  sfu.pending_sr = pending_sr;

  return sfu;
}

} // namespace sched_nr_impl
} // namespace srsenb
