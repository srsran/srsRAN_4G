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

#include "lib/include/srsran/adt/circular_array.h"
#include "sched_nr_interface.h"
#include "sched_nr_ue.h"

namespace srsenb {
namespace sched_nr_impl {

using pdsch_list = sched_nr_interface::pdsch_list;
using pusch_list = sched_nr_interface::pusch_list;

struct pdcch_t {};
struct pdsch_t {};
struct pusch_t {};
struct pucch_t {};

struct phy_slot_grid {
  pdcchmask_t                                              pdcch_tot_mask;
  rbgmask_t                                                pdsch_tot_mask;
  rbgmask_t                                                ul_tot_mask;
  pdsch_list                                               pdsch_grants;
  pusch_list                                               pusch_grants;
  srsran::bounded_vector<pucch_t, SCHED_NR_MAX_PDSCH_DATA> pucch_grants;
};
using phy_cell_rb_grid = srsran::circular_array<phy_slot_grid, TTIMOD_SZ>;

struct slot_ue_grid {
  phy_slot_grid* pdcch_slot;
  phy_slot_grid* pdsch_slot;
  phy_slot_grid* pusch_slot;
  phy_slot_grid* pucch_slot;
  pdcch_t*       pdcch_alloc = nullptr;
  pdsch_t*       pdsch_alloc = nullptr;
  pusch_t*       pusch_alloc = nullptr;
  pucch_t*       pucch_alloc = nullptr;

  slot_ue_grid(phy_slot_grid& pdcch_sl, phy_slot_grid& pdsch_sl, phy_slot_grid& pusch_sl, phy_slot_grid& pucch_sl) :
    pdcch_slot(&pdcch_sl), pdsch_slot(&pdsch_sl), pusch_slot(&pusch_sl), pucch_slot(&pucch_sl)
  {}
};

class rb_alloc_grid
{
public:
  slot_ue_grid get_slot_ue_grid(tti_point pdcch_tti, uint8_t K0, uint8_t K1, uint8_t K2)
  {
    phy_slot_grid& pdcch_slot = phy_grid[pdcch_tti.to_uint()];
    phy_slot_grid& pdsch_slot = phy_grid[(pdcch_tti + K0).to_uint()];
    phy_slot_grid& pucch_slot = phy_grid[(pdcch_tti + K0 + K1).to_uint()];
    phy_slot_grid& pusch_slot = phy_grid[(pdcch_tti + K2).to_uint()];
    return slot_ue_grid{pdcch_slot, pdsch_slot, pusch_slot, pucch_slot};
  }

private:
  phy_cell_rb_grid phy_grid;
};

/// Error code of alloc attempt
enum class alloc_result { success, sch_collision, no_grant_space, no_rnti_opportunity };
inline const char* to_string(alloc_result res)
{
  return "";
}

class slot_sched
{
public:
  explicit slot_sched(const sched_cell_params& cfg_, phy_cell_rb_grid& phy_grid_);
  void         new_tti(tti_point tti_rx_);
  alloc_result alloc_pdsch(slot_ue& ue, const rbgmask_t& dl_mask);
  alloc_result alloc_pusch(slot_ue& ue, const rbgmask_t& dl_mask);

  void generate_dcis();

  const sched_cell_params& cfg;

private:
  srslog::basic_logger& logger;
  phy_cell_rb_grid      phy_grid;

  tti_point tti_rx;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_RB_GRID_H
