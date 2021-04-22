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

#ifndef SRSRAN_SCHED_COMMON_H
#define SRSRAN_SCHED_COMMON_H

#include "srsran/adt/bounded_bitset.h"
#include "srsran/common/tti_point.h"
#include "srsran/interfaces/sched_interface.h"

namespace srsenb {

/***********************
 *     Constants
 **********************/

constexpr float    tti_duration_ms = 1;
constexpr uint32_t NOF_AGGR_LEVEL  = 4;

/***********************
 *   Helper Types
 **********************/

/// List of CCE start positions in PDCCH
using cce_position_list = srsran::bounded_vector<uint32_t, 6>;

/// Map {L} -> list of CCE positions
using cce_cfi_position_table = std::array<cce_position_list, NOF_AGGR_LEVEL>;

/// Map {cfi, L} -> list of CCE positions
using cce_sf_position_table = std::array<std::array<cce_position_list, NOF_AGGR_LEVEL>, SRSRAN_NOF_CFI>;

/// Map {sf, cfi, L} -> list of CCE positions
using cce_frame_position_table = std::array<cce_sf_position_table, SRSRAN_NOF_SF_X_FRAME>;

/// structs to bundle together all the sched arguments, and share them with all the sched sub-components
class sched_cell_params_t
{
  struct regs_deleter {
    void operator()(srsran_regs_t* p);
  };

public:
  bool set_cfg(uint32_t                             enb_cc_idx_,
               const sched_interface::cell_cfg_t&   cfg_,
               const sched_interface::sched_args_t& sched_args);
  // convenience getters
  uint32_t nof_prbs_to_rbgs(uint32_t nof_prbs) const { return srsran::ceil_div(nof_prbs, P); }
  uint32_t nof_prb() const { return cfg.cell.nof_prb; }
  uint32_t get_dl_lb_nof_re(tti_point tti_tx_dl, uint32_t nof_prbs_alloc) const;
  uint32_t get_dl_nof_res(srsran::tti_point tti_tx_dl, const srsran_dci_dl_t& dci, uint32_t cfi) const;

  uint32_t                                     enb_cc_idx       = 0;
  sched_interface::cell_cfg_t                  cfg              = {};
  srsran_pucch_cfg_t                           pucch_cfg_common = {};
  const sched_interface::sched_args_t*         sched_cfg        = nullptr;
  std::unique_ptr<srsran_regs_t, regs_deleter> regs;
  cce_sf_position_table                        common_locations = {};
  cce_frame_position_table                     rar_locations    = {};
  std::array<uint32_t, SRSRAN_NOF_CFI>         nof_cce_table    = {}; ///< map cfix -> nof cces in PDCCH
  uint32_t                                     P                = 0;
  uint32_t                                     nof_rbgs         = 0;

  using dl_nof_re_table = srsran::bounded_vector<
      std::array<std::array<std::array<uint32_t, SRSRAN_NOF_CFI>, SRSRAN_NOF_SLOTS_PER_SF>, SRSRAN_NOF_SF_X_FRAME>,
      SRSRAN_MAX_PRB>;
  using dl_lb_nof_re_table = std::array<srsran::bounded_vector<uint32_t, SRSRAN_MAX_PRB>, SRSRAN_NOF_SF_X_FRAME>;

  /// Table of nof REs
  dl_nof_re_table nof_re_table;
  /// Cached computation of Lower bound of nof REs
  dl_lb_nof_re_table nof_re_lb_table;
};

//! Bitmask used for CCE allocations
using pdcch_mask_t = srsran::bounded_bitset<sched_interface::max_cce, true>;

//! Bitmask that stores the allocared DL RBGs
using rbgmask_t = srsran::bounded_bitset<25, true>;

//! Bitmask that stores the allocated UL PRBs
using prbmask_t = srsran::bounded_bitset<100, true>;

//! Struct to express a {min,...,max} range of RBGs
struct prb_interval;
struct rbg_interval : public srsran::interval<uint32_t> {
  using interval::interval;
  static rbg_interval rbgmask_to_rbgs(const rbgmask_t& mask);
};

/// Struct to express a {min,...,max} range of PRBs
struct prb_interval : public srsran::interval<uint32_t> {
  using interval::interval;
  static prb_interval rbgs_to_prbs(const rbg_interval& rbgs, uint32_t cell_nof_prb);
  static prb_interval riv_to_prbs(uint32_t riv, uint32_t nof_prbs, int nof_vrbs = -1);
};

/// Type of Allocation stored in PDSCH/PUSCH
enum class alloc_type_t { DL_BC, DL_PCCH, DL_RAR, DL_DATA, UL_DATA };
inline bool is_dl_ctrl_alloc(alloc_type_t a)
{
  return a == alloc_type_t::DL_BC or a == alloc_type_t::DL_PCCH or a == alloc_type_t::DL_RAR;
}

} // namespace srsenb

namespace fmt {

template <>
struct formatter<srsenb::rbg_interval> : public formatter<srsran::interval<uint32_t> > {};
template <>
struct formatter<srsenb::prb_interval> : public formatter<srsran::interval<uint32_t> > {};

} // namespace fmt

#endif // SRSRAN_SCHED_COMMON_H
