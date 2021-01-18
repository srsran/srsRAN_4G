/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_ue_cell.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include <numeric>

namespace srsenb {

/*******************************************************
 *                sched_ue_cell
 *******************************************************/

sched_ue_cell::sched_ue_cell(uint16_t rnti_, const sched_cell_params_t& cell_cfg_, tti_point current_tti_) :
  rnti(rnti_),
  cell_cfg(&cell_cfg_),
  dci_locations(generate_cce_location_table(rnti_, cell_cfg_)),
  harq_ent(SCHED_MAX_HARQ_PROC, SCHED_MAX_HARQ_PROC),
  tpc_fsm(cell_cfg->nof_prb(), cell_cfg->cfg.target_ul_sinr, cell_cfg->cfg.enable_phr_handling),
  fixed_mcs_dl(cell_cfg_.sched_cfg->pdsch_mcs),
  fixed_mcs_ul(cell_cfg_.sched_cfg->pusch_mcs),
  current_tti(current_tti_)
{
  max_aggr_level = cell_cfg->sched_cfg->max_aggr_level >= 0 ? cell_cfg->sched_cfg->max_aggr_level : 3;
}

void sched_ue_cell::set_ue_cfg(const sched_interface::ue_cfg_t& ue_cfg_)
{
  cfg_tti   = current_tti;
  ue_cfg    = &ue_cfg_;
  ue_cc_idx = -1;
  for (size_t i = 0; i < ue_cfg_.supported_cc_list.size(); ++i) {
    if (ue_cfg_.supported_cc_list[i].enb_cc_idx == cell_cfg->enb_cc_idx) {
      ue_cc_idx = i;
    }
  }

  // set max mcs
  max_mcs_ul = cell_cfg->sched_cfg->pusch_max_mcs >= 0 ? cell_cfg->sched_cfg->pusch_max_mcs : 28u;
  if (cell_cfg->cfg.enable_64qam) {
    const uint32_t max_64qam_mcs[] = {20, 24, 28};
    max_mcs_ul                     = std::min(max_mcs_ul, max_64qam_mcs[(size_t)ue_cfg->support_ul64qam]);
  }
  max_mcs_dl = cell_cfg->sched_cfg->pdsch_max_mcs >= 0 ? std::min(cell_cfg->sched_cfg->pdsch_max_mcs, 28) : 28u;
  if (ue_cfg->use_tbs_index_alt) {
    max_mcs_dl = std::min(max_mcs_dl, 27u);
  }

  // Update carrier state
  if (ue_cc_idx == 0) {
    if (cc_state() == cc_st::idle) {
      reset();
      // PCell is always active
      cc_state_ = cc_st::active;

      // set initial DL CQI
      dl_cqi = cell_cfg->cfg.initial_dl_cqi;
    }
  } else {
    // SCell case
    switch (cc_state()) {
      case cc_st::activating:
      case cc_st::active:
        if (not ue_cfg->supported_cc_list[ue_cc_idx].active) {
          cc_state_ = cc_st::deactivating;
          log_h->info("SCHED: Deactivating rnti=0x%x, SCellIndex=%d...\n", rnti, ue_cc_idx);
        }
        break;
      case cc_st::deactivating:
      case cc_st::idle:
        if (ue_cfg->supported_cc_list[ue_cc_idx].active) {
          reset();
          cc_state_ = cc_st::activating;
          dl_cqi    = 0;
          log_h->info("SCHED: Activating rnti=0x%x, SCellIndex=%d...\n", rnti, ue_cc_idx);
        }
        break;
      default:
        break;
    }
  }
}

void sched_ue_cell::new_tti(tti_point tti_rx)
{
  current_tti = tti_rx;

  // Check if cell state needs to be updated
  if (ue_cc_idx > 0 and cc_state_ == cc_st::deactivating) {
    // wait for all ACKs to be received before completely deactivating SCell
    if (current_tti > to_tx_dl_ack(cfg_tti)) {
      cc_state_ = cc_st::idle;
    }
  }
}

void sched_ue_cell::reset()
{
  dl_ri         = 0;
  dl_ri_tti_rx  = tti_point{};
  dl_pmi        = 0;
  dl_pmi_tti_rx = tti_point{};
  dl_cqi        = 1;
  dl_cqi_tti_rx = tti_point{};
  dl_cqi_rx     = false;
  ul_cqi        = 1;
  ul_cqi_tti_rx = tti_point{};
  harq_ent.reset();
}

void sched_ue_cell::finish_tti(tti_point tti_rx)
{
  // reset PIDs with pending data or blocked
  harq_ent.reset_pending_data(tti_rx);
}

void sched_ue_cell::set_dl_cqi(tti_point tti_rx, uint32_t dl_cqi_)
{
  dl_cqi        = dl_cqi_;
  dl_cqi_tti_rx = tti_rx;
  dl_cqi_rx     = dl_cqi_rx or dl_cqi > 0;
  if (ue_cc_idx > 0 and cc_state_ == cc_st::activating and dl_cqi_rx) {
    // Wait for SCell to receive a positive CQI before activating it
    cc_state_ = cc_st::active;
    log_h->info("SCHED: SCell index=%d is now active\n", ue_cc_idx);
  }
}

} // namespace srsenb
