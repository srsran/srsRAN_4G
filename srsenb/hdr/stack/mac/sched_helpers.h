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

#ifndef SRSRAN_SCHED_HELPERS_H
#define SRSRAN_SCHED_HELPERS_H

#include "srsenb/hdr/stack/mac/sched_common.h"
#include "srsran/interfaces/sched_interface.h"
#include "srsran/srslog/srslog.h"

namespace srsenb {

inline uint32_t aggr_level(uint32_t aggr_idx)
{
  return 1u << aggr_idx;
}

/// Obtain rvidx from nof retxs. This value is stored in DCI
inline uint32_t get_rvidx(uint32_t retx_idx)
{
  const static uint32_t rv_idx[4] = {0, 2, 3, 1};
  return rv_idx[retx_idx % 4];
}

/// Obtain nof retxs from rvidx.
inline uint32_t get_nof_retx(uint32_t rv_idx)
{
  const static uint32_t nof_retxs[4] = {0, 3, 1, 2};
  return nof_retxs[rv_idx % 4];
}

/// convert cell nof PRBs to nof RBGs
inline uint32_t cell_nof_prb_to_rbg(uint32_t nof_prbs)
{
  switch (nof_prbs) {
    case 6:
      return 6;
    case 15:
      return 8;
    case 25:
      return 13;
    case 50:
      return 17;
    case 75:
      return 19;
    case 100:
      return 25;
    default:
      srslog::fetch_basic_logger("MAC").error("Provided nof PRBs not valid");
      return 0;
  }
}

/// convert cell nof RBGs to nof PRBs
inline uint32_t cell_nof_rbg_to_prb(uint32_t nof_rbgs)
{
  switch (nof_rbgs) {
    case 6:
      return 6;
    case 8:
      return 15;
    case 13:
      return 25;
    case 17:
      return 50;
    case 19:
      return 75;
    case 25:
      return 100;
    default:
      srslog::fetch_basic_logger("MAC").error("Provided nof PRBs not valid");
      return 0;
  }
}

/**
 * Count number of PRBs present in a DL RBG mask
 * @param cell_nof_prb cell nof prbs
 * @param P cell ratio prb/rbg
 * @param bitmask DL RBG mask
 * @return number of prbs
 */
inline uint32_t count_prb_per_tb(const rbgmask_t& bitmask)
{
  uint32_t Nprb    = cell_nof_rbg_to_prb(bitmask.size());
  uint32_t P       = srsran_ra_type0_P(Nprb);
  uint32_t nof_prb = P * bitmask.count();
  if (bitmask.test(bitmask.size() - 1)) {
    nof_prb -= bitmask.size() * P - Nprb;
  }
  return nof_prb;
}

inline uint32_t count_prb_per_tb_approx(uint32_t nof_rbgs, uint32_t cell_nof_prb)
{
  uint32_t P = srsran_ra_type0_P(cell_nof_prb);
  return std::min(nof_rbgs * P, cell_nof_prb);
}

cce_frame_position_table generate_cce_location_table(uint16_t rnti, const sched_cell_params_t& cell_cfg);

/**
 * Generate possible CCE locations a user can use to allocate DCIs
 * @param regs Regs data for the given cell configuration
 * @param location Result of the CCE location computation.
 * @param cfi Number of control symbols used for the PDCCH
 * @param sf_idx subframe index specific to the tx TTI (relevant only for data and RAR transmissions)
 * @param rnti identity of the user (invalid RNTI for RAR and BC transmissions)
 */
void generate_cce_location(srsran_regs_t*          regs,
                           cce_cfi_position_table& locations,
                           uint32_t                cfi,
                           uint32_t                sf_idx = 0,
                           uint16_t                rnti   = SRSRAN_INVALID_RNTI);

/// Obtains TB size *in bytes* for a given MCS and nof allocated prbs
inline uint32_t get_tbs_bytes(uint32_t mcs, uint32_t nof_alloc_prb, bool use_tbs_index_alt, bool is_ul)
{
  int tbs_idx = srsran_ra_tbs_idx_from_mcs(mcs, use_tbs_index_alt, is_ul);
  assert(tbs_idx != SRSRAN_ERROR);

  int tbs = srsran_ra_tbs_from_idx((uint32_t)tbs_idx, nof_alloc_prb);
  assert(tbs != SRSRAN_ERROR);

  return (uint32_t)tbs / 8U;
}

/// Find lowest DCI aggregation level supported by the UE spectral efficiency
uint32_t get_aggr_level(uint32_t nof_bits,
                        uint32_t dl_cqi,
                        uint32_t max_aggr_lvl,
                        uint32_t cell_nof_prb,
                        bool     use_tbs_index_alt);

/*******************************************************
 *              RB mask helper functions
 *******************************************************/

bool is_contiguous(const rbgmask_t& mask);

/*******************************************************
 *          sched_interface helper functions
 *******************************************************/

inline bool operator==(const sched_interface::ue_cfg_t::cc_cfg_t& lhs, const sched_interface::ue_cfg_t::cc_cfg_t& rhs)
{
  return lhs.enb_cc_idx == rhs.enb_cc_idx and lhs.active == rhs.active;
}

/// sanity check the UE CC configuration
int check_ue_cfg_correctness(const sched_interface::ue_cfg_t& ue_cfg);

/// Logs DL MAC PDU contents
void log_dl_cc_results(srslog::basic_logger&                  logger,
                       uint32_t                               enb_cc_idx,
                       const sched_interface::dl_sched_res_t& result);

/// Logs PHICH contents
void log_phich_cc_results(srslog::basic_logger&                  logger,
                          uint32_t                               enb_cc_idx,
                          const sched_interface::ul_sched_res_t& result);

const char* to_string(sched_interface::ue_bearer_cfg_t::direction_t dir);

} // namespace srsenb

#endif // SRSRAN_SCHED_HELPERS_H
