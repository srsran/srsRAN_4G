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

#ifndef SRSRAN_SCHED_NR_RB_GRID_H
#define SRSRAN_SCHED_NR_RB_GRID_H

#include "sched_nr_interface.h"
#include "sched_nr_ue.h"

namespace srsenb {
namespace sched_nr_impl {

class slot_grid
{
public:
  explicit slot_grid(const sched_cell_params& cfg_);
  void new_tti(tti_point tti_rx_, sched_nr_res_t& sched_res_);
  bool alloc_pdsch(slot_ue& ue, const rbgmask_t& dl_mask);
  bool alloc_pusch(slot_ue& ue, const rbgmask_t& dl_mask);

  void generate_dcis();

  tti_point tti_tx_dl() const { return tti_rx + TX_ENB_DELAY; }
  tti_point tti_tx_ul() const { return tti_tx_dl() + cfg.cell_cfg.K2; }

  const sched_cell_params& cfg;

private:
  tti_point       tti_rx;
  rbgmask_t       pdsch_mask;
  rbgmask_t       pusch_mask;
  sched_nr_res_t* sched_res = nullptr;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_RB_GRID_H
