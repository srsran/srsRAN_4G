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

#ifndef SRSRAN_SCHED_NR_GRANT_ALLOCATOR_H
#define SRSRAN_SCHED_NR_GRANT_ALLOCATOR_H

#include "../sched_common.h"
#include "lib/include/srsran/adt/circular_array.h"
#include "sched_nr_helpers.h"
#include "sched_nr_interface.h"
#include "sched_nr_pdcch.h"
#include "sched_nr_ue.h"

namespace srsenb {
namespace sched_nr_impl {

// typedefs
using dl_sched_rar_info_t = sched_nr_interface::rar_info_t;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const static size_t MAX_CORESET_PER_BWP = 3;
using slot_coreset_list                 = std::array<srsran::optional<coreset_region>, MAX_CORESET_PER_BWP>;

using pdsch_list_t     = srsran::bounded_vector<pdsch_t, MAX_GRANTS>;
using sched_rar_list_t = sched_nr_interface::sched_rar_list_t;
using pucch_list_t     = srsran::bounded_vector<pucch_t, MAX_GRANTS>;

struct harq_ack_t {
  const srsran::phy_cfg_nr_t* phy_cfg;
  srsran_harq_ack_resource_t  res;
};
using harq_ack_list_t = srsran::bounded_vector<harq_ack_t, MAX_GRANTS>;

/// save data for scheduler to keep track of previous allocations
/// This only contains information about a given slot
struct bwp_slot_grid {
  uint32_t            slot_idx = 0;
  const bwp_params_t* cfg      = nullptr;

  bwp_rb_bitmap     dl_prbs;
  bwp_rb_bitmap     ul_prbs;
  dl_sched_res_t    dl;
  ul_sched_t        ul;
  slot_coreset_list coresets;
  harq_ack_list_t   pending_acks;

  srsran::unique_pool_ptr<tx_harq_softbuffer> rar_softbuffer;

  bwp_slot_grid() = default;
  explicit bwp_slot_grid(const bwp_params_t& bwp_params, uint32_t slot_idx_);
  void reset();

  bool is_dl() const { return cfg->slots[slot_idx].is_dl; }
  bool is_ul() const { return cfg->slots[slot_idx].is_ul; }
};

struct bwp_res_grid {
  explicit bwp_res_grid(const bwp_params_t& bwp_cfg_);

  bwp_slot_grid&       operator[](slot_point tti) { return slots[tti.to_uint() % slots.capacity()]; };
  const bwp_slot_grid& operator[](slot_point tti) const { return slots[tti.to_uint() % slots.capacity()]; };
  uint32_t             id() const { return cfg->bwp_id; }
  uint32_t             nof_prbs() const { return cfg->cfg.rb_width; }

  const bwp_params_t* cfg = nullptr;

private:
  // TTIMOD_SZ is the longest allocation in the future
  srsran::bounded_vector<bwp_slot_grid, TTIMOD_SZ> slots;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Class responsible for jointly filling the DL/UL sched result fields and allocate RB/PDCCH resources in the RB grid
 * to avoid potential RB/PDCCH collisions
 */
class bwp_slot_allocator
{
public:
  explicit bwp_slot_allocator(bwp_res_grid& bwp_grid_, slot_point pdcch_slot_, slot_ue_map_t& ues_);

  alloc_result alloc_si(uint32_t aggr_idx, uint32_t si_idx, uint32_t si_ntx, const prb_interval& prbs);
  alloc_result alloc_rar_and_msg3(uint16_t                                ra_rnti,
                                  uint32_t                                aggr_idx,
                                  prb_interval                            interv,
                                  srsran::const_span<dl_sched_rar_info_t> pending_rars);
  alloc_result alloc_pdsch(slot_ue& ue, const prb_grant& dl_grant);
  alloc_result alloc_pusch(slot_ue& ue, const prb_grant& dl_mask);

  slot_point          get_pdcch_tti() const { return pdcch_slot; }
  slot_point          get_tti_rx() const { return pdcch_slot - TX_ENB_DELAY; }
  const bwp_res_grid& res_grid() const { return bwp_grid; }

  const bwp_params_t& cfg;

private:
  alloc_result
               verify_pdsch_space(bwp_slot_grid& pdsch_grid, bwp_slot_grid& pdcch_grid, bwp_slot_grid* uci_grid = nullptr) const;
  alloc_result verify_pusch_space(bwp_slot_grid& pusch_grid, bwp_slot_grid* pdcch_grid = nullptr) const;
  alloc_result verify_ue_cfg(const ue_carrier_params_t& ue_cfg, harq_proc* harq) const;

  srslog::basic_logger& logger;
  bwp_res_grid&         bwp_grid;

  slot_point     pdcch_slot;
  slot_ue_map_t& slot_ues;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_GRANT_ALLOCATOR_H
