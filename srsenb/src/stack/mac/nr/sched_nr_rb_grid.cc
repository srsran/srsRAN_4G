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

slot_grid::slot_grid(const sched_cell_params& cfg_) :
  cfg(cfg_), pdsch_mask(cfg.cell_cfg.nof_rbg), pusch_mask(cfg.cell_cfg.nof_rbg)
{}

void slot_grid::new_tti(tti_point tti_rx_, sched_nr_res_t& sched_res_)
{
  tti_rx    = tti_rx_;
  sched_res = &sched_res_;

  pdsch_mask.reset();
  pusch_mask.reset();
  *sched_res = {};
}

bool slot_grid::alloc_pdsch(slot_ue& ue, const rbgmask_t& dl_mask)
{
  const uint32_t tbs = 100, mcs = 20;
  if (ue.h_dl == nullptr) {
    return false;
  }
  if ((pdsch_mask & dl_mask).any()) {
    return false;
  }
  if (sched_res->dl_res.data.full()) {
    return false;
  }
  if (not ue.h_dl->new_tx(tti_tx_dl(), dl_mask, mcs, cfg.cell_cfg.K1)) {
    return false;
  }

  pdsch_mask |= dl_mask;
  sched_res->dl_res.data.emplace_back();
  sched_nr_data_t& data = sched_res->dl_res.data.back();
  data.tbs.resize(1);
  data.tbs[0] = tbs;

  return true;
}

bool slot_grid::alloc_pusch(slot_ue& ue, const rbgmask_t& ul_mask)
{
  const uint32_t tbs = 100, mcs = 20;
  if ((pusch_mask & ul_mask).any()) {
    return false;
  }
  if (sched_res->ul_res.pusch.full()) {
    return false;
  }
  if (not ue.h_ul->new_tx(tti_tx_ul(), ul_mask, mcs, 0)) {
    return false;
  }

  pusch_mask |= ul_mask;
  sched_res->ul_res.pusch.emplace_back();
  sched_nr_data_t& data = sched_res->ul_res.pusch.back();
  data.tbs.resize(1);
  data.tbs[0] = tbs;

  return true;
}

void slot_grid::generate_dcis() {}

} // namespace sched_nr_impl
} // namespace srsenb