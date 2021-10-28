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

#include "srsenb/hdr/stack/mac/nr/sched_nr_grant_allocator.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_cell.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

bwp_slot_grid::bwp_slot_grid(const bwp_params_t& bwp_cfg_, uint32_t slot_idx_) :
  dl_prbs(bwp_cfg_.cfg.rb_width, bwp_cfg_.cfg.start_rb, bwp_cfg_.cfg.pdsch.rbg_size_cfg_1),
  ul_prbs(bwp_cfg_.cfg.rb_width, bwp_cfg_.cfg.start_rb, bwp_cfg_.cfg.pdsch.rbg_size_cfg_1),
  slot_idx(slot_idx_),
  cfg(&bwp_cfg_),
  rar_softbuffer(harq_softbuffer_pool::get_instance().get_tx(bwp_cfg_.cfg.rb_width))
{
  for (uint32_t cs_idx = 0; cs_idx < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; ++cs_idx) {
    if (cfg->cfg.pdcch.coreset_present[cs_idx]) {
      uint32_t cs_id = cfg->cfg.pdcch.coreset[cs_idx].id;
      coresets[cs_id].emplace(*cfg, cs_id, slot_idx_, dl_pdcchs, ul_pdcchs);
    }
  }
}

void bwp_slot_grid::reset()
{
  for (auto& coreset : coresets) {
    if (coreset.has_value()) {
      coreset->reset();
    }
  }
  dl_prbs.reset();
  ul_prbs.reset();
  dl_pdcchs.clear();
  ul_pdcchs.clear();
  pdschs.clear();
  puschs.clear();
  pending_acks.clear();
  pucch.clear();
  ssb.clear();
  nzp_csi_rs.clear();
  rar.clear();
}

bwp_res_grid::bwp_res_grid(const bwp_params_t& bwp_cfg_) : cfg(&bwp_cfg_)
{
  for (uint32_t sl = 0; sl < slots.capacity(); ++sl) {
    slots.emplace_back(*cfg, sl % static_cast<uint32_t>(SRSRAN_NSLOTS_PER_FRAME_NR(0u)));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bwp_slot_allocator::bwp_slot_allocator(bwp_res_grid& bwp_grid_) :
  logger(bwp_grid_.cfg->logger), cfg(*bwp_grid_.cfg), bwp_grid(bwp_grid_)
{}

alloc_result bwp_slot_allocator::alloc_si(uint32_t aggr_idx, uint32_t si_idx, uint32_t si_ntx, const prb_interval& prbs)
{
  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[pdcch_slot];
  if (not bwp_pdcch_slot.is_dl()) {
    logger.warning("SCHED: Trying to allocate PDSCH in TDD non-DL slot index=%d", bwp_pdcch_slot.slot_idx);
    return alloc_result::no_sch_space;
  }
  pdcch_dl_list_t& pdsch_grants = bwp_pdcch_slot.dl_pdcchs;
  if (pdsch_grants.full()) {
    logger.warning("SCHED: Maximum number of DL allocations reached");
    return alloc_result::no_grant_space;
  }
  if (bwp_pdcch_slot.dl_prbs.collides(prbs)) {
    return alloc_result::sch_collision;
  }

  // TODO: Allocate PDCCH and PDSCH

  return alloc_result::success;
}

alloc_result bwp_slot_allocator::alloc_rar_and_msg3(uint16_t                                ra_rnti,
                                                    uint32_t                                aggr_idx,
                                                    prb_interval                            interv,
                                                    srsran::const_span<dl_sched_rar_info_t> pending_rars)
{
  static const uint32_t msg3_nof_prbs = 3, m = 0;

  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[pdcch_slot];
  if (not bwp_pdcch_slot.ssb.empty()) {
    // TODO: support concurrent PDSCH and SSB
    logger.info("SCHED: skipping ra-rnti=0x%x RAR allocation. Cause: concurrent PDSCH and SSB not yet supported",
                ra_rnti);
    return alloc_result::no_sch_space;
  }
  slot_point     msg3_slot     = pdcch_slot + cfg.pusch_ra_list[m].msg3_delay;
  bwp_slot_grid& bwp_msg3_slot = bwp_grid[msg3_slot];
  alloc_result   ret           = verify_pusch_space(bwp_msg3_slot, nullptr);
  if (ret != alloc_result::success) {
    return ret;
  }
  ret = verify_pdsch_space(bwp_pdcch_slot, bwp_pdcch_slot);
  if (ret != alloc_result::success) {
    return ret;
  }
  if (bwp_pdcch_slot.rar.full()) {
    return alloc_result::no_grant_space;
  }
  if (pending_rars.size() > MAX_GRANTS) {
    logger.error("SCHED: Trying to allocate too many Msg3 grants in a single slot (%zd)", pending_rars.size());
    return alloc_result::invalid_grant_params;
  }
  for (auto& rar : pending_rars) {
    if (not slot_ues->contains(rar.temp_crnti)) {
      logger.info("SCHED: Postponing rnti=0x%x RAR allocation. Cause: The ue object not yet fully created",
                  rar.temp_crnti);
      return alloc_result::no_rnti_opportunity;
    }
  }

  // Check DL RB collision
  if (bwp_pdcch_slot.dl_prbs.collides(interv)) {
    logger.debug("SCHED: Provided RBG mask collides with allocation previously made.");
    return alloc_result::sch_collision;
  }

  // Check Msg3 RB collision
  uint32_t     total_ul_nof_prbs = msg3_nof_prbs * pending_rars.size();
  uint32_t     total_ul_nof_rbgs = srsran::ceil_div(total_ul_nof_prbs, get_P(bwp_grid.nof_prbs(), false));
  prb_interval msg3_rbs          = find_empty_interval_of_length(bwp_msg3_slot.ul_prbs.prbs(), total_ul_nof_rbgs);
  if (msg3_rbs.length() < total_ul_nof_rbgs) {
    logger.debug("SCHED: No space in PUSCH for Msg3.");
    return alloc_result::sch_collision;
  }

  // Find PDCCH position
  const uint32_t coreset_id      = cfg.cfg.pdcch.ra_search_space.coreset_id;
  const uint32_t search_space_id = cfg.cfg.pdcch.ra_search_space.id;
  if (not bwp_pdcch_slot.coresets[coreset_id]->alloc_dci(pdcch_grant_type_t::rar, aggr_idx, search_space_id, nullptr)) {
    // Could not find space in PDCCH
    logger.debug("SCHED: No space in PDCCH for DL tx.");
    return alloc_result::no_cch_space;
  }

  // RAR allocation successful.
  bwp_pdcch_slot.dl_prbs |= interv;
  // Generate DCI for RAR with given RA-RNTI
  pdcch_dl_t& pdcch = bwp_pdcch_slot.dl_pdcchs.back();
  if (not fill_dci_rar(interv, ra_rnti, *bwp_grid.cfg, pdcch.dci)) {
    // Cancel on-going PDCCH allocation
    bwp_pdcch_slot.coresets[coreset_id]->rem_last_dci();
    return alloc_result::invalid_coderate;
  }
  auto& phy_cfg = (*slot_ues)[pending_rars[0].temp_crnti].cfg->phy();
  pdcch.dci_cfg = phy_cfg.get_dci_cfg();
  // Generate RAR PDSCH
  // TODO: Properly fill Msg3 grants
  bwp_pdcch_slot.pdschs.emplace_back();
  pdsch_t&          pdsch = bwp_pdcch_slot.pdschs.back();
  srsran_slot_cfg_t slot_cfg;
  slot_cfg.idx = pdcch_slot.to_uint();
  bool success = phy_cfg.get_pdsch_cfg(slot_cfg, pdcch.dci, pdsch.sch);
  srsran_assert(success, "Error converting DCI to grant");
  pdsch.sch.grant.tb[0].softbuffer.tx = bwp_pdcch_slot.rar_softbuffer->get();

  // Generate Msg3 grants in PUSCH
  uint32_t  last_msg3 = msg3_rbs.start();
  const int mcs = 0, max_harq_msg3_retx = 4;
  slot_cfg.idx = msg3_slot.to_uint();
  bwp_pdcch_slot.rar.emplace_back();
  sched_nr_interface::rar_t& rar_out = bwp_pdcch_slot.rar.back();
  for (const dl_sched_rar_info_t& grant : pending_rars) {
    slot_ue& ue = (*slot_ues)[grant.temp_crnti];

    // Generate RAR grant
    rar_out.grants.emplace_back();
    auto& rar_grant = rar_out.grants.back();
    rar_grant.data  = grant;
    prb_interval msg3_interv{last_msg3, last_msg3 + msg3_nof_prbs};
    last_msg3 += msg3_nof_prbs;
    ue.h_ul = ue.harq_ent->find_empty_ul_harq();
    success = ue.h_ul->new_tx(msg3_slot, msg3_slot, msg3_interv, mcs, max_harq_msg3_retx);
    srsran_assert(success, "Failed to allocate Msg3");
    fill_dci_msg3(ue, *bwp_grid.cfg, rar_grant.msg3_dci);

    // Generate PUSCH
    bwp_msg3_slot.puschs.emplace_back();
    pusch_t& pusch = bwp_msg3_slot.puschs.back();
    success        = ue.cfg->phy().get_pusch_cfg(slot_cfg, rar_grant.msg3_dci, pusch.sch);
    srsran_assert(success, "Error converting DCI to PUSCH grant");
    pusch.sch.grant.tb[0].softbuffer.rx = ue.h_ul->get_softbuffer().get();
    ue.h_ul->set_tbs(pusch.sch.grant.tb[0].tbs);
  }
  bwp_msg3_slot.ul_prbs.add(msg3_rbs);

  return alloc_result::success;
}

// ue is the UE (1 only) that will be allocated
// func computes the grant allocation for this UE
alloc_result bwp_slot_allocator::alloc_pdsch(slot_ue& ue, const prb_grant& dl_grant)
{
  if (ue.cfg->active_bwp().bwp_id != bwp_grid.cfg->bwp_id) {
    logger.warning(
        "SCHED: Trying to allocate PDSCH for rnti=0x%x in inactive BWP id=%d", ue.rnti, ue.cfg->active_bwp().bwp_id);
    return alloc_result::no_rnti_opportunity;
  }
  if (ue.h_dl == nullptr) {
    logger.warning("SCHED: Trying to allocate PDSCH for rnti=0x%x with no available HARQs", ue.rnti);
    return alloc_result::no_rnti_opportunity;
  }
  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[ue.pdcch_slot];
  bwp_slot_grid& bwp_pdsch_slot = bwp_grid[ue.pdsch_slot];
  bwp_slot_grid& bwp_uci_slot   = bwp_grid[ue.uci_slot]; // UCI : UL control info
  alloc_result   result         = verify_pdsch_space(bwp_pdsch_slot, bwp_pdcch_slot);
  if (result != alloc_result::success) {
    return result;
  }
  if (bwp_uci_slot.pending_acks.full()) {
    logger.warning("SCHED: PDSCH allocation for rnti=0x%x failed due to lack of space for respective ACK", ue.rnti);
    return alloc_result::no_grant_space;
  }
  if (bwp_pdsch_slot.dl_prbs.collides(dl_grant)) {
    return alloc_result::sch_collision;
  }
  if (not bwp_pdcch_slot.ssb.empty()) {
    // TODO: support concurrent PDSCH and SSB
    logger.info("SCHED: skipping rnti=0x%x PDSCH allocation. Cause: concurrent PDSCH and SSB not yet supported",
                ue.rnti);
    return alloc_result::no_sch_space;
  }

  // Find space in PUCCH
  // TODO

  // Find space and allocate PDCCH
  const uint32_t aggr_idx = 2;
  // Choose the ss_id the highest number of candidates
  uint32_t ss_id = 0, max_nof_candidates = 0;
  for (uint32_t i = 0; i < 3; ++i) {
    uint32_t nof_candidates = ue.cfg->cce_pos_list(i, pdcch_slot.slot_idx(), aggr_idx).size();
    if (nof_candidates > max_nof_candidates) {
      ss_id              = i;
      max_nof_candidates = nof_candidates;
    }
  }
  uint32_t coreset_id = ue.cfg->phy().pdcch.search_space[ss_id].coreset_id;
  if (not bwp_pdcch_slot.coresets[coreset_id]->alloc_dci(pdcch_grant_type_t::dl_data, aggr_idx, ss_id, &ue)) {
    // Could not find space in PDCCH
    return alloc_result::no_cch_space;
  }

  // Allocate HARQ
  int mcs = ue.cfg->fixed_pdsch_mcs();
  if (ue.h_dl->empty()) {
    bool ret = ue.h_dl->new_tx(ue.pdsch_slot, ue.uci_slot, dl_grant, mcs, 4);
    srsran_assert(ret, "Failed to allocate DL HARQ");
  } else {
    bool ret = ue.h_dl->new_retx(ue.pdsch_slot, ue.uci_slot, dl_grant);
    mcs      = ue.h_dl->mcs();
    srsran_assert(ret, "Failed to allocate DL HARQ retx");
  }

  // Allocation Successful

  const static float max_R = 0.93;
  while (true) {
    // Generate PDCCH
    pdcch_dl_t& pdcch = bwp_pdcch_slot.dl_pdcchs.back();
    fill_dl_dci_ue_fields(ue, *bwp_grid.cfg, ss_id, pdcch.dci.ctx.location, pdcch.dci);
    pdcch.dci.pucch_resource = 0;
    pdcch.dci.dai            = std::count_if(bwp_uci_slot.pending_acks.begin(),
                                  bwp_uci_slot.pending_acks.end(),
                                  [&ue](const harq_ack_t& p) { return p.res.rnti == ue.rnti; });
    pdcch.dci.dai %= 4;
    pdcch.dci_cfg = ue.cfg->phy().get_dci_cfg();

    // Generate PUCCH
    bwp_uci_slot.pending_acks.emplace_back();
    bwp_uci_slot.pending_acks.back().phy_cfg = &ue.cfg->phy();
    srsran_assert(ue.cfg->phy().get_pdsch_ack_resource(pdcch.dci, bwp_uci_slot.pending_acks.back().res),
                  "Error getting ack resource");

    // Generate PDSCH
    bwp_pdsch_slot.dl_prbs |= dl_grant;
    bwp_pdsch_slot.pdschs.emplace_back();
    pdsch_t&          pdsch = bwp_pdsch_slot.pdschs.back();
    srsran_slot_cfg_t slot_cfg;
    slot_cfg.idx = ue.pdsch_slot.to_uint();
    bool ret     = ue.cfg->phy().get_pdsch_cfg(slot_cfg, pdcch.dci, pdsch.sch);
    srsran_assert(ret, "Error converting DCI to grant");

    pdsch.sch.grant.tb[0].softbuffer.tx = ue.h_dl->get_softbuffer().get();
    pdsch.data[0]                       = ue.h_dl->get_tx_pdu()->get();
    if (ue.h_dl->nof_retx() == 0) {
      ue.h_dl->set_tbs(pdsch.sch.grant.tb[0].tbs); // update HARQ with correct TBS
    } else {
      srsran_assert(pdsch.sch.grant.tb[0].tbs == (int)ue.h_dl->tbs(), "The TBS did not remain constant in retx");
    }
    if (ue.h_dl->nof_retx() > 0 or bwp_pdsch_slot.pdschs.back().sch.grant.tb[0].R_prime < max_R or mcs <= 0) {
      break;
    }
    // Decrease MCS if first tx and rate is too high
    mcs--;
    ue.h_dl->set_mcs(mcs);
    bwp_pdsch_slot.pdschs.pop_back();
    bwp_uci_slot.pending_acks.pop_back();
  }
  if (mcs == 0) {
    logger.warning("Couldn't find mcs that leads to R<0.9");
  }

  return alloc_result::success;
}

alloc_result bwp_slot_allocator::alloc_pusch(slot_ue& ue, const prb_grant& ul_prbs)
{
  auto&        bwp_pdcch_slot = bwp_grid[ue.pdcch_slot];
  auto&        bwp_pusch_slot = bwp_grid[ue.pusch_slot];
  alloc_result ret            = verify_pusch_space(bwp_pusch_slot, &bwp_pdcch_slot);
  if (ret != alloc_result::success) {
    return ret;
  }

  if (ue.h_ul == nullptr) {
    logger.warning("SCHED: Trying to allocate PUSCH for rnti=0x%x with no available HARQs", ue.rnti);
    return alloc_result::no_rnti_opportunity;
  }
  pdcch_ul_list_t& pdcchs = bwp_pdcch_slot.ul_pdcchs;
  if (bwp_pusch_slot.ul_prbs.collides(ul_prbs)) {
    return alloc_result::sch_collision;
  }
  const uint32_t aggr_idx = 2;
  // Choose the ss_id the highest number of candidates
  uint32_t ss_id = 0, max_nof_candidates = 0;
  for (uint32_t i = 0; i < 3; ++i) {
    uint32_t nof_candidates = ue.cfg->cce_pos_list(i, pdcch_slot.slot_idx(), aggr_idx).size();
    if (nof_candidates > max_nof_candidates) {
      ss_id              = i;
      max_nof_candidates = nof_candidates;
    }
  }
  uint32_t coreset_id = ue.cfg->phy().pdcch.search_space[ss_id].coreset_id;
  if (not bwp_pdcch_slot.coresets[coreset_id].value().alloc_dci(pdcch_grant_type_t::ul_data, aggr_idx, ss_id, &ue)) {
    // Could not find space in PDCCH
    return alloc_result::no_cch_space;
  }

  if (ue.h_ul->empty()) {
    int  mcs     = ue.cfg->fixed_pusch_mcs();
    int  tbs     = 100;
    bool success = ue.h_ul->new_tx(ue.pusch_slot, ue.pusch_slot, ul_prbs, mcs, ue.cfg->ue_cfg()->maxharq_tx);
    srsran_assert(success, "Failed to allocate UL HARQ");
  } else {
    bool success = ue.h_ul->new_retx(ue.pusch_slot, ue.pusch_slot, ul_prbs);
    srsran_assert(success, "Failed to allocate UL HARQ retx");
  }

  // Allocation Successful
  // Generate PDCCH
  pdcch_ul_t& pdcch = pdcchs.back();
  fill_ul_dci_ue_fields(ue, *bwp_grid.cfg, ss_id, pdcch.dci.ctx.location, pdcch.dci);
  pdcch.dci_cfg = ue.cfg->phy().get_dci_cfg();
  // Generate PUSCH
  bwp_pusch_slot.ul_prbs |= ul_prbs;
  bwp_pusch_slot.puschs.emplace_back();
  pusch_t&          pusch = bwp_pusch_slot.puschs.back();
  srsran_slot_cfg_t slot_cfg;
  slot_cfg.idx = ue.pusch_slot.to_uint();
  pusch.pid    = ue.h_ul->pid;
  bool success = ue.cfg->phy().get_pusch_cfg(slot_cfg, pdcch.dci, pusch.sch);
  srsran_assert(success, "Error converting DCI to PUSCH grant");
  pusch.sch.grant.tb[0].softbuffer.rx = ue.h_ul->get_softbuffer().get();
  if (ue.h_ul->nof_retx() == 0) {
    ue.h_ul->set_tbs(pusch.sch.grant.tb[0].tbs); // update HARQ with correct TBS
  } else {
    srsran_assert(pusch.sch.grant.tb[0].tbs == (int)ue.h_ul->tbs(), "The TBS did not remain constant in retx");
  }

  return alloc_result::success;
}

alloc_result bwp_slot_allocator::verify_pdsch_space(bwp_slot_grid& bwp_pdsch, bwp_slot_grid& bwp_pdcch) const
{
  if (not bwp_pdsch.is_dl() or not bwp_pdcch.is_dl()) {
    logger.warning("SCHED: Trying to allocate PDSCH in TDD non-DL slot index=%d", bwp_pdsch.slot_idx);
    return alloc_result::no_sch_space;
  }
  if (bwp_pdcch.dl_pdcchs.full()) {
    logger.warning("SCHED: Maximum number of DL PDCCH allocations reached");
    return alloc_result::no_cch_space;
  }
  if (bwp_pdsch.pdschs.full()) {
    logger.warning("SCHED: Maximum number of DL PDSCH grants reached");
    return alloc_result::no_sch_space;
  }
  return alloc_result::success;
}

alloc_result bwp_slot_allocator::verify_pusch_space(bwp_slot_grid& pusch_grid, bwp_slot_grid* pdcch_grid) const
{
  if (not pusch_grid.is_ul()) {
    logger.warning("SCHED: Trying to allocate PUSCH in TDD non-UL slot index=%d", pusch_grid.slot_idx);
    return alloc_result::no_sch_space;
  }
  if (pdcch_grid != nullptr) {
    // DCI needed
    if (not pdcch_grid->is_dl()) {
      logger.warning("SCHED: Trying to allocate PDCCH in TDD non-DL slot index=%d", pdcch_grid->slot_idx);
      return alloc_result::no_sch_space;
    }
    if (pdcch_grid->ul_pdcchs.full()) {
      logger.warning("SCHED: Maximum number of PUSCH allocations reached");
      return alloc_result::no_grant_space;
    }
  }
  if (pusch_grid.puschs.full()) {
    logger.warning("SCHED: Maximum number of PUSCH allocations reached");
    return alloc_result::no_grant_space;
  }
  return alloc_result::success;
}

} // namespace sched_nr_impl
} // namespace srsenb