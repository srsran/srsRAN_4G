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

namespace srsenb {
namespace sched_nr_impl {

using pdsch_grant = sched_nr_interface::pdsch_grant;
using pusch_grant = sched_nr_interface::pusch_grant;

slot_sched::slot_sched(const sched_cell_params& cfg_, phy_cell_rb_grid& phy_grid_) :
  logger(srslog::fetch_basic_logger("MAC")), cfg(cfg_), phy_grid(phy_grid_)
{}

void slot_sched::new_tti(tti_point tti_rx_)
{
  tti_rx = tti_rx_;
}

alloc_result slot_sched::alloc_pdsch(slot_ue& ue, const rbgmask_t& dl_mask)
{
  if (ue.h_dl == nullptr) {
    logger.warning("SCHED: Trying to allocate PDSCH for rnti=0x%x with no available HARQs", ue.rnti);
    return alloc_result::no_rnti_opportunity;
  }
  pdsch_list& pdsch_grants = phy_grid[ue.pdsch_tti.to_uint()].pdsch_grants;
  if (pdsch_grants.full()) {
    logger.warning("SCHED: Maximum number of DL allocations reached");
    return alloc_result::no_grant_space;
  }
  rbgmask_t& pdsch_mask = phy_grid[ue.pdsch_tti.to_uint()].pdsch_tot_mask;
  if ((pdsch_mask & dl_mask).any()) {
    return alloc_result::sch_collision;
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

  // Allocation Successful
  pdsch_grants.emplace_back();
  pdsch_grant& grant = pdsch_grants.back();
  grant.dci.ctx.rnti = ue.rnti;
  grant.dci.pid      = ue.h_dl->pid;
  grant.bitmap       = dl_mask;
  pdsch_mask |= dl_mask;

  return alloc_result::success;
}

alloc_result slot_sched::alloc_pusch(slot_ue& ue, const rbgmask_t& ul_mask)
{
  if (ue.h_ul == nullptr) {
    logger.warning("SCHED: Trying to allocate PUSCH for rnti=0x%x with no available HARQs", ue.rnti);
    return alloc_result::no_rnti_opportunity;
  }
  pusch_list& pusch_grants = phy_grid[ue.pusch_tti.to_uint()].pusch_grants;
  if (pusch_grants.full()) {
    logger.warning("SCHED: Maximum number of UL allocations reached");
    return alloc_result::no_grant_space;
  }
  rbgmask_t& pusch_mask = phy_grid[ue.pusch_tti.to_uint()].ul_tot_mask;
  if ((pusch_mask & ul_mask).any()) {
    return alloc_result::sch_collision;
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
  pusch_grants.emplace_back();
  pusch_grant& grant = pusch_grants.back();
  grant.dci.ctx.rnti = ue.rnti;
  grant.dci.pid      = ue.h_dl->pid;
  grant.dci.mcs      = mcs;
  grant.bitmap       = ul_mask;
  pusch_mask |= ul_mask;

  return alloc_result::success;
}

void slot_sched::generate_dcis() {}

} // namespace sched_nr_impl
} // namespace srsenb