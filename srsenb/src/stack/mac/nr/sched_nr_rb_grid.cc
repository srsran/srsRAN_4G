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

#include "srsenb/hdr/stack/mac/nr/sched_nr_rb_grid.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_phy.h"

namespace srsenb {
namespace sched_nr_impl {

#define NUMEROLOGY_IDX 0

bwp_slot_grid::bwp_slot_grid(const bwp_params& bwp_cfg_, uint32_t slot_idx_) :
  dl_rbgs(bwp_cfg_.N_rbg),
  ul_rbgs(bwp_cfg_.N_rbg),
  slot_idx(slot_idx_),
  cfg(&bwp_cfg_),
  is_dl(srsran_tdd_nr_is_dl(&bwp_cfg_.cell_cfg.tdd, NUMEROLOGY_IDX, slot_idx_)),
  is_ul(srsran_tdd_nr_is_ul(&bwp_cfg_.cell_cfg.tdd, NUMEROLOGY_IDX, slot_idx_))
{
  const uint32_t coreset_id = 0; // Note: for now only one coreset per BWP supported
  coresets.emplace_back(*cfg, coreset_id, slot_idx_, dl_pdcchs, ul_pdcchs);
}

void bwp_slot_grid::reset()
{
  for (auto& coreset : coresets) {
    coreset.reset();
  }
  dl_rbgs.reset();
  ul_rbgs.reset();
  dl_pdcchs.clear();
  ul_pdcchs.clear();
  pucchs.clear();
}

bwp_res_grid::bwp_res_grid(const bwp_params& bwp_cfg_) : cfg(&bwp_cfg_)
{
  for (uint32_t sl = 0; sl < slots.capacity(); ++sl) {
    slots.emplace_back(*cfg, sl % static_cast<uint32_t>(SRSRAN_NSLOTS_PER_FRAME_NR(0u)));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bwp_slot_allocator::bwp_slot_allocator(bwp_res_grid& bwp_grid_) :
  logger(srslog::fetch_basic_logger("MAC")), cfg(*bwp_grid_.cfg), bwp_grid(bwp_grid_)
{}

alloc_result bwp_slot_allocator::alloc_rar(uint32_t                                    aggr_idx,
                                           const srsenb::sched_nr_impl::pending_rar_t& rar,
                                           srsenb::sched_nr_impl::rbg_interval         interv,
                                           uint32_t                                    nof_grants)
{
  static const uint32_t msg3_nof_prbs = 3;

  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[pdcch_tti];
  bwp_slot_grid& bwp_msg3_slot  = bwp_grid[pdcch_tti + 4];

  if (bwp_pdcch_slot.dl_pdcchs.full()) {
    logger.warning("SCHED: Maximum number of DL allocations reached");
    return alloc_result::no_grant_space;
  }

  // Check DL RB collision
  rbgmask_t& pdsch_mask = bwp_pdcch_slot.dl_rbgs;
  rbgmask_t  dl_mask(pdsch_mask.size());
  dl_mask.fill(interv.start(), interv.stop());
  if ((pdsch_mask & dl_mask).any()) {
    logger.debug("SCHED: Provided RBG mask collides with allocation previously made.");
    return alloc_result::sch_collision;
  }

  // Check Msg3 RB collision
  uint32_t     total_ul_nof_prbs = msg3_nof_prbs * nof_grants;
  uint32_t     total_ul_nof_rbgs = srsran::ceil_div(total_ul_nof_prbs, get_P(bwp_grid.nof_prbs(), false));
  rbg_interval msg3_rbgs         = find_empty_rbg_interval(bwp_msg3_slot.ul_rbgs, total_ul_nof_rbgs);
  if (msg3_rbgs.length() < total_ul_nof_rbgs) {
    logger.debug("SCHED: No space in PUSCH for Msg3.");
    return alloc_result::sch_collision;
  }

  // Find PDCCH position
  const uint32_t coreset_id = 0;
  if (not bwp_pdcch_slot.coresets[coreset_id].alloc_dci(pdcch_grant_type_t::rar, aggr_idx, nullptr)) {
    // Could not find space in PDCCH
    logger.debug("SCHED: No space in PDCCH for DL tx.");
    return alloc_result::no_cch_space;
  }

  // Generate DCI for RAR
  pdcch_dl_t& pdcch = bwp_pdcch_slot.dl_pdcchs.back();
  if (not fill_dci_rar(interv, *bwp_grid.cfg, pdcch.dci)) {
    // Cancel on-going PDCCH allocation
    bwp_pdcch_slot.coresets[coreset_id].rem_last_dci();
    return alloc_result::invalid_coderate;
  }

  // RAR allocation successful.
  bwp_pdcch_slot.dl_rbgs.fill(interv.start(), interv.stop());

  return alloc_result::success;
}

alloc_result bwp_slot_allocator::alloc_pdsch(slot_ue& ue, const rbgmask_t& dl_mask)
{
  if (ue.h_dl == nullptr) {
    logger.warning("SCHED: Trying to allocate PDSCH for rnti=0x%x with no available HARQs", ue.rnti);
    return alloc_result::no_rnti_opportunity;
  }
  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[ue.pdcch_tti];
  bwp_slot_grid& bwp_pdsch_slot = bwp_grid[ue.pdsch_tti];
  bwp_slot_grid& bwp_uci_slot   = bwp_grid[ue.uci_tti];
  if (not bwp_pdsch_slot.is_dl) {
    logger.warning("SCHED: Trying to allocate PDSCH in TDD non-DL slot index=%d", bwp_pdsch_slot.slot_idx);
    return alloc_result::no_sch_space;
  }
  pdcch_dl_list_t& pdsch_grants = bwp_pdsch_slot.dl_pdcchs;
  if (pdsch_grants.full()) {
    logger.warning("SCHED: Maximum number of DL allocations reached");
    return alloc_result::no_grant_space;
  }
  rbgmask_t& pdsch_mask = bwp_pdsch_slot.dl_rbgs;
  if ((pdsch_mask & dl_mask).any()) {
    return alloc_result::sch_collision;
  }
  const uint32_t aggr_idx = 2, coreset_id = 0;
  if (not bwp_pdcch_slot.coresets[coreset_id].alloc_dci(pdcch_grant_type_t::dl_data, aggr_idx, &ue)) {
    // Could not find space in PDCCH
    return alloc_result::no_cch_space;
  }

  int mcs = -1, tbs = -1;
  if (ue.h_dl->empty()) {
    mcs      = 20;
    tbs      = 100;
    bool ret = ue.h_dl->new_tx(ue.pdsch_tti, ue.uci_tti, dl_mask, mcs, tbs, 4);
    srsran_assert(ret, "Failed to allocate DL HARQ");
  } else {
    bool ret = ue.h_dl->new_retx(ue.pdsch_tti, ue.uci_tti, dl_mask, &mcs, &tbs);
    srsran_assert(ret, "Failed to allocate DL HARQ retx");
  }

  pucch_resource_grant pucch_res = find_pucch_resource(ue, bwp_uci_slot.ul_rbgs, tbs);
  if (pucch_res.rnti != SRSRAN_INVALID_RNTI) {
    // Could not find space in PUCCH for HARQ-ACK
    bwp_pdcch_slot.coresets[coreset_id].rem_last_dci();
    return alloc_result::no_cch_space;
  }

  // Allocation Successful
  pdcch_dl_t& pdcch = bwp_pdcch_slot.dl_pdcchs.back();
  fill_dci_ue_cfg(ue, dl_mask, *bwp_grid.cfg, pdcch.dci);
  pdsch_mask |= dl_mask;
  bwp_uci_slot.pucchs.emplace_back();
  pucch_grant& pucch = bwp_uci_slot.pucchs.back();
  pucch.resource     = pucch_res;
  bwp_uci_slot.ul_rbgs.set(
      ue.cfg->phy_cfg.pucch.sets[pucch_res.resource_set_id].resources[pucch_res.resource_id].starting_prb);

  return alloc_result::success;
}

alloc_result bwp_slot_allocator::alloc_pusch(slot_ue& ue, const rbgmask_t& ul_mask)
{
  if (ue.h_ul == nullptr) {
    logger.warning("SCHED: Trying to allocate PUSCH for rnti=0x%x with no available HARQs", ue.rnti);
    return alloc_result::no_rnti_opportunity;
  }
  auto& bwp_pdcch_slot = bwp_grid[ue.pdcch_tti];
  auto& bwp_pusch_slot = bwp_grid[ue.pusch_tti];
  if (not bwp_pusch_slot.is_ul) {
    logger.warning("SCHED: Trying to allocate PUSCH in TDD non-UL slot index=%d", bwp_pusch_slot.slot_idx);
    return alloc_result::no_sch_space;
  }
  pdcch_ul_list_t& pdcchs = bwp_pdcch_slot.ul_pdcchs;
  if (pdcchs.full()) {
    logger.warning("SCHED: Maximum number of UL allocations reached");
    return alloc_result::no_grant_space;
  }
  rbgmask_t& pusch_mask = bwp_pusch_slot.ul_rbgs;
  if ((pusch_mask & ul_mask).any()) {
    return alloc_result::sch_collision;
  }
  const uint32_t aggr_idx = 2, coreset_id = 0;
  if (not bwp_pdcch_slot.coresets[coreset_id].alloc_dci(pdcch_grant_type_t::ul_data, aggr_idx, &ue)) {
    // Could not find space in PDCCH
    return alloc_result::no_cch_space;
  }

  int mcs = -1, tbs = -1;
  if (ue.h_ul->empty()) {
    mcs      = 20;
    tbs      = 100;
    bool ret = ue.h_ul->new_tx(ue.pusch_tti, ue.pusch_tti, ul_mask, mcs, tbs, ue.cfg->maxharq_tx);
    srsran_assert(ret, "Failed to allocate UL HARQ");
  } else {
    srsran_assert(ue.h_ul->new_retx(ue.pusch_tti, ue.pusch_tti, ul_mask, &mcs, &tbs),
                  "Failed to allocate UL HARQ retx");
  }

  // Allocation Successful
  pdcch_ul_t& pdcch = pdcchs.back();
  fill_dci_ue_cfg(ue, ul_mask, *bwp_grid.cfg, pdcch.dci);
  pusch_mask |= ul_mask;

  return alloc_result::success;
}

} // namespace sched_nr_impl
} // namespace srsenb