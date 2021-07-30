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

slot_ue::slot_ue(uint16_t rnti_, slot_point slot_rx_, uint32_t cc_) : rnti(rnti_), slot_rx(slot_rx_), cc(cc_) {}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_carrier::ue_carrier(uint16_t rnti_, const ue_cfg_t& uecfg_, const sched_cell_params& cell_params_) :
  rnti(rnti_),
  cc(cell_params_.cc),
  bwp_cfg(rnti_, cell_params_.bwps[0], uecfg_),
  cell_params(cell_params_),
  harq_ent(cell_params_.nof_prb())
{}

void ue_carrier::new_slot(slot_point pdcch_slot, const ue_cfg_t& uecfg_)
{
  if (bwp_cfg.ue_cfg() != &uecfg_) {
    bwp_cfg = bwp_ue_cfg(rnti, cell_params.bwps[0], uecfg_);
  }
  harq_ent.new_slot(pdcch_slot - TX_ENB_DELAY);
}

slot_ue ue_carrier::try_reserve(slot_point pdcch_slot)
{
  slot_point slot_rx = pdcch_slot - TX_ENB_DELAY;

  // copy cc-specific parameters and find available HARQs
  slot_ue sfu(rnti, slot_rx, cc);
  sfu.cfg           = &bwp_cfg;
  sfu.harq_ent      = &harq_ent;
  sfu.pdcch_slot    = pdcch_slot;
  const uint32_t k0 = 0;
  sfu.pdsch_slot    = sfu.pdcch_slot + k0;
  uint32_t k1 =
      sfu.cfg->phy()
          .harq_ack.dl_data_to_ul_ack[sfu.pdsch_slot.slot_idx() % sfu.cfg->phy().harq_ack.nof_dl_data_to_ul_ack];
  sfu.uci_slot   = sfu.pdsch_slot + k1;
  uint32_t k2    = bwp_cfg.active_bwp().pusch_ra_list[0].K;
  sfu.pusch_slot = sfu.pdcch_slot + k2;
  sfu.dl_cqi     = dl_cqi;
  sfu.ul_cqi     = ul_cqi;

  const srsran_tdd_config_nr_t& tdd_cfg = cell_params.cell_cfg.tdd;
  if (srsran_tdd_nr_is_dl(&tdd_cfg, 0, sfu.pdsch_slot.slot_idx())) {
    // If DL enabled
    sfu.h_dl = harq_ent.find_pending_dl_retx();
    if (sfu.h_dl == nullptr) {
      sfu.h_dl = harq_ent.find_empty_dl_harq();
    }
  }
  if (srsran_tdd_nr_is_ul(&tdd_cfg, 0, sfu.pusch_slot.slot_idx())) {
    // If UL enabled
    sfu.h_ul = harq_ent.find_pending_ul_retx();
    if (sfu.h_ul == nullptr) {
      sfu.h_ul = harq_ent.find_empty_ul_harq();
    }
  }

  if (sfu.h_dl == nullptr and sfu.h_ul == nullptr) {
    // there needs to be at least one available HARQ for newtx/retx
    sfu.release();
    return sfu;
  }
  return sfu;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue::ue(uint16_t rnti_, const ue_cfg_t& cfg, const sched_params& sched_cfg_) :
  rnti(rnti_), sched_cfg(sched_cfg_), ue_cfg(cfg)
{
  for (uint32_t cc = 0; cc < cfg.carriers.size(); ++cc) {
    if (cfg.carriers[cc].active) {
      carriers[cc].reset(new ue_carrier(rnti, cfg, sched_cfg.cells[cc]));
    }
  }
}

void ue::set_cfg(const ue_cfg_t& cfg)
{
  ue_cfg = cfg;
}

slot_ue ue::try_reserve(slot_point pdcch_slot, uint32_t cc)
{
  if (carriers[cc] == nullptr) {
    return slot_ue();
  }
  slot_ue sfu = carriers[cc]->try_reserve(pdcch_slot);
  if (sfu.empty()) {
    return slot_ue();
  }

  // set UE-common parameters
  sfu.pending_sr = pending_sr;

  return sfu;
}

} // namespace sched_nr_impl
} // namespace srsenb
