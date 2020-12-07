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

#ifndef SRSLTE_SCHED_COMMON_H
#define SRSLTE_SCHED_COMMON_H

#include "srslte/adt/bounded_bitset.h"
#include "srslte/common/tti_point.h"
#include "srslte/interfaces/sched_interface.h"

namespace srsenb {

/***********************
 *     Constants
 **********************/

constexpr float tti_duration_ms = 1;

/***********************
 *   Helper Types
 **********************/

//! Struct used to store possible CCE locations.
struct sched_dci_cce_t {
  uint32_t cce_start[4][6]; ///< Stores starting CCE for each aggr level index and CCE location index
  uint32_t nof_loc[4];      ///< Number of possible CCE locations for each aggregation level index
};

/// structs to bundle together all the sched arguments, and share them with all the sched sub-components
class sched_cell_params_t
{
  struct regs_deleter {
    void operator()(srslte_regs_t* p);
  };

public:
  bool set_cfg(uint32_t                             enb_cc_idx_,
               const sched_interface::cell_cfg_t&   cfg_,
               const sched_interface::sched_args_t& sched_args);
  // convenience getters
  uint32_t prb_to_rbg(uint32_t nof_prbs) const { return (nof_prbs + (P - 1)) / P; }
  uint32_t nof_prb() const { return cfg.cell.nof_prb; }

  uint32_t                                       enb_cc_idx = 0;
  sched_interface::cell_cfg_t                    cfg        = {};
  const sched_interface::sched_args_t*           sched_cfg  = nullptr;
  std::unique_ptr<srslte_regs_t, regs_deleter>   regs;
  std::array<sched_dci_cce_t, 3>                 common_locations = {};
  std::array<std::array<sched_dci_cce_t, 10>, 3> rar_locations    = {};
  std::array<uint32_t, 3>                        nof_cce_table    = {}; ///< map cfix -> nof cces in PDCCH
  uint32_t                                       P                = 0;
  uint32_t                                       nof_rbgs         = 0;
};

//! Bitmask used for CCE allocations
using pdcch_mask_t = srslte::bounded_bitset<sched_interface::max_cce, true>;

//! Bitmask that stores the allocared DL RBGs
using rbgmask_t = srslte::bounded_bitset<25, true>;

//! Bitmask that stores the allocated UL PRBs
using prbmask_t = srslte::bounded_bitset<100, true>;

//! Struct to express a {min,...,max} range of RBGs
struct prb_interval;
struct rbg_interval : public srslte::interval<uint32_t> {
  using interval::interval;
  static rbg_interval prbs_to_rbgs(const prb_interval& prbs, uint32_t P);
  static rbg_interval rbgmask_to_rbgs(const rbgmask_t& mask);
};

//! Struct to express a {min,...,max} range of PRBs
struct prb_interval : public srslte::interval<uint32_t> {
  using interval::interval;

  static prb_interval rbgs_to_prbs(const rbg_interval& rbgs, uint32_t P);
  static prb_interval riv_to_prbs(uint32_t riv, uint32_t nof_prbs, int nof_vrbs = -1);
};

} // namespace srsenb

#endif // SRSLTE_SCHED_COMMON_H
