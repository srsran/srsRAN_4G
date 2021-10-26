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

#ifndef SRSRAN_SCHED_NR_CFG_H
#define SRSRAN_SCHED_NR_CFG_H

#include "sched_nr_interface.h"
#include "sched_nr_rb.h"
#include "srsenb/hdr/common/common_enb.h"

namespace srsenb {

const static size_t SCHED_NR_MAX_USERS     = SRSENB_MAX_UES;
const static size_t SCHED_NR_NOF_SUBFRAMES = 10;
const static size_t SCHED_NR_NOF_HARQS     = 16;
static const size_t MAX_NOF_AGGR_LEVELS    = 5;

namespace sched_nr_impl {

constexpr static size_t MAX_GRANTS = sched_nr_interface::MAX_GRANTS;

using pdcch_dl_t         = mac_interface_phy_nr::pdcch_dl_t;
using pdcch_ul_t         = mac_interface_phy_nr::pdcch_ul_t;
using pdsch_t            = mac_interface_phy_nr::pdsch_t;
using pusch_t            = mac_interface_phy_nr::pusch_t;
using pucch_t            = mac_interface_phy_nr::pucch_t;
using pdcch_dl_list_t    = srsran::bounded_vector<pdcch_dl_t, MAX_GRANTS>;
using pdcch_ul_list_t    = srsran::bounded_vector<pdcch_ul_t, MAX_GRANTS>;
using pucch_list_t       = srsran::bounded_vector<pucch_t, MAX_GRANTS>;
using pusch_list_t       = srsran::bounded_vector<pusch_t, MAX_GRANTS>;
using nzp_csi_rs_list    = srsran::bounded_vector<srsran_csi_rs_nzp_resource_t, mac_interface_phy_nr::MAX_NZP_CSI_RS>;
using ssb_t              = mac_interface_phy_nr::ssb_t;
using ssb_list           = srsran::bounded_vector<ssb_t, mac_interface_phy_nr::MAX_SSB>;
using sched_args_t       = sched_nr_interface::sched_args_t;
using cell_cfg_t         = sched_nr_interface::cell_cfg_t;
using bwp_cfg_t          = sched_nr_interface::bwp_cfg_t;
using ue_cfg_t           = sched_nr_interface::ue_cfg_t;
using ue_cc_cfg_t        = sched_nr_interface::ue_cc_cfg_t;
using pdcch_cce_pos_list = srsran::bounded_vector<uint32_t, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR>;
using bwp_cce_pos_list   = std::array<std::array<pdcch_cce_pos_list, MAX_NOF_AGGR_LEVELS>, SRSRAN_NOF_SF_X_FRAME>;
using dl_sched_t         = sched_nr_interface::dl_sched_t;
using ul_sched_t         = sched_nr_interface::ul_res_t;
using dl_sched_res_t     = sched_nr_interface::dl_res_t;

/// Generate list of CCE locations for UE based on coreset and search space configurations
void get_dci_locs(const srsran_coreset_t&      coreset,
                  const srsran_search_space_t& search_space,
                  uint16_t                     rnti,
                  bwp_cce_pos_list&            cce_locs);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Structure that extends the sched_nr_interface::bwp_cfg_t passed by upper layers with other
/// derived BWP-specific params
struct bwp_params_t {
  const uint32_t      bwp_id;
  const uint32_t      cc;
  const bwp_cfg_t&    cfg;
  const cell_cfg_t&   cell_cfg;
  const sched_args_t& sched_cfg;

  // derived params
  srslog::basic_logger& logger;
  uint32_t              P;
  uint32_t              N_rbg;
  uint32_t              nof_prb() const { return cell_cfg.carrier.nof_prb; }

  /// Table specifying if a slot has DL or UL enabled
  struct slot_cfg {
    bool is_dl;
    bool is_ul;
  };
  srsran::bounded_vector<slot_cfg, SRSRAN_NOF_SF_X_FRAME> slots;

  struct pusch_ra_time_cfg {
    uint32_t msg3_delay; ///< Includes K2 and delta. See TS 36.214 6.1.2.1.1-2/4/5
    uint32_t K;
    uint32_t S;
    uint32_t L;
  };
  std::vector<pusch_ra_time_cfg> pusch_ra_list;

  bwp_cce_pos_list rar_cce_list;

  bwp_params_t(const cell_cfg_t& cell, const sched_args_t& sched_cfg_, uint32_t cc, uint32_t bwp_id);
};

/// Structure packing a single cell config params, and sched args
struct cell_params_t {
  const uint32_t            cc;
  const cell_cfg_t          cfg;
  const sched_args_t&       sched_args;
  std::vector<bwp_params_t> bwps;

  cell_params_t(uint32_t cc_, const cell_cfg_t& cell, const sched_args_t& sched_cfg_);

  uint32_t nof_prb() const { return cfg.carrier.nof_prb; }
};

/// Structure packing both the sched args and all gNB NR cell configurations
struct sched_params {
  sched_args_t               sched_cfg;
  std::vector<cell_params_t> cells;

  sched_params() = default;
  explicit sched_params(const sched_args_t& sched_cfg_);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Configuration of a UE for a given BWP
class bwp_ue_cfg
{
public:
  bwp_ue_cfg() = default;
  explicit bwp_ue_cfg(uint16_t rnti, const bwp_params_t& bwp_cfg, const ue_cfg_t& uecfg_);

  const ue_cfg_t*              ue_cfg() const { return cfg_; }
  const srsran::phy_cfg_nr_t&  phy() const { return cfg_->phy_cfg; }
  const bwp_params_t&          active_bwp() const { return *bwp_cfg; }
  srsran::const_span<uint32_t> cce_pos_list(uint32_t search_id, uint32_t slot_idx, uint32_t aggr_idx) const
  {
    if (cce_positions_list.size() > ss_id_to_cce_idx[search_id]) {
      auto& lst = cce_pos_list(search_id);
      return lst[slot_idx][aggr_idx];
    }
    return srsran::const_span<uint32_t>{};
  }
  const bwp_cce_pos_list& cce_pos_list(uint32_t search_id) const
  {
    return cce_positions_list[ss_id_to_cce_idx[search_id]];
  }
  uint32_t get_k1(slot_point pdsch_slot) const
  {
    if (phy().duplex.mode == SRSRAN_DUPLEX_MODE_TDD) {
      return phy().harq_ack.dl_data_to_ul_ack[pdsch_slot.to_uint() % phy().duplex.tdd.pattern1.period_ms];
    }
    return phy().harq_ack.dl_data_to_ul_ack[pdsch_slot.to_uint() % phy().harq_ack.nof_dl_data_to_ul_ack];
  }
  int fixed_pdsch_mcs() const { return bwp_cfg->sched_cfg.fixed_dl_mcs; }
  int fixed_pusch_mcs() const { return bwp_cfg->sched_cfg.fixed_ul_mcs; }

private:
  uint16_t            rnti    = SRSRAN_INVALID_RNTI;
  const ue_cfg_t*     cfg_    = nullptr;
  const bwp_params_t* bwp_cfg = nullptr;

  std::vector<bwp_cce_pos_list>                              cce_positions_list;
  std::array<uint32_t, SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE> ss_id_to_cce_idx;
};

class ue_cfg_extended : public ue_cfg_t
{
public:
  struct search_space_params {
    const srsran_search_space_t* cfg;
    bwp_cce_pos_list             cce_positions;
  };
  struct coreset_params {
    srsran_coreset_t*     cfg = nullptr;
    std::vector<uint32_t> ss_list;
  };
  struct bwp_params {
    std::array<srsran::optional<search_space_params>, SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE> ss_list;
    std::vector<coreset_params>                                                             coresets;
  };
  struct cc_params {
    srsran::bounded_vector<bwp_params, SCHED_NR_MAX_BWP_PER_CELL> bwps;
  };

  ue_cfg_extended() = default;
  explicit ue_cfg_extended(uint16_t rnti, const ue_cfg_t& uecfg);

  const bwp_cce_pos_list& get_dci_pos_list(uint32_t cc, uint32_t bwp_id, uint32_t search_space_id) const
  {
    return cc_params[cc].bwps[bwp_id].ss_list[search_space_id]->cce_positions;
  }

  uint16_t               rnti;
  std::vector<cc_params> cc_params;
};

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_CFG_H
