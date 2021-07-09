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

#include "../sched_common.h"
#include "lib/include/srsran/adt/circular_array.h"
#include "sched_nr_interface.h"
#include "sched_nr_pdcch.h"
#include "sched_nr_phy.h"
#include "sched_nr_ue.h"

namespace srsenb {
namespace sched_nr_impl {

struct pending_rar_t;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const static size_t MAX_CORESET_PER_BWP = 3;
using slot_coreset_list                 = std::array<srsran::optional<coreset_region>, MAX_CORESET_PER_BWP>;

using pdsch_t      = mac_interface_phy_nr::pdsch_t;
using pdsch_list_t = srsran::bounded_vector<pdsch_t, MAX_GRANTS>;

struct bwp_slot_grid {
  uint32_t          slot_idx;
  const bwp_params* cfg;

  bool              is_dl, is_ul;
  bwp_rb_bitmap     dl_prbs;
  bwp_rb_bitmap     ul_prbs;
  pdcch_dl_list_t   dl_pdcchs;
  pdcch_ul_list_t   ul_pdcchs;
  pdsch_list_t      pdschs;
  slot_coreset_list coresets;
  pucch_list_t      pucchs;
  pusch_list_t      puschs;

  bwp_slot_grid() = default;
  explicit bwp_slot_grid(const bwp_params& bwp_params, uint32_t slot_idx_);
  void reset();
};

struct bwp_res_grid {
  bwp_res_grid(const bwp_params& bwp_cfg_);

  bwp_slot_grid&       operator[](tti_point tti) { return slots[tti.to_uint() % slots.capacity()]; };
  const bwp_slot_grid& operator[](tti_point tti) const { return slots[tti.to_uint() % slots.capacity()]; };
  uint32_t             id() const { return cfg->bwp_id; }
  uint32_t             nof_prbs() const { return cfg->cfg.rb_width; }

  const bwp_params* cfg = nullptr;

private:
  srsran::bounded_vector<bwp_slot_grid, TTIMOD_SZ> slots;
};

class bwp_slot_allocator
{
public:
  explicit bwp_slot_allocator(bwp_res_grid& bwp_grid_);

  void new_slot(tti_point pdcch_tti_) { pdcch_tti = pdcch_tti_; }

  alloc_result alloc_rar(uint32_t aggr_idx, const pending_rar_t& rar, prb_interval interv, uint32_t max_nof_grants);
  alloc_result alloc_pdsch(slot_ue& ue, const prb_grant& dl_grant);
  alloc_result alloc_pusch(slot_ue& ue, const rbgmask_t& dl_mask);

  tti_point           get_pdcch_tti() const { return pdcch_tti; }
  const bwp_res_grid& res_grid() const { return bwp_grid; }

  const bwp_params& cfg;

private:
  srslog::basic_logger& logger;
  bwp_res_grid&         bwp_grid;

  tti_point pdcch_tti;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_RB_GRID_H
