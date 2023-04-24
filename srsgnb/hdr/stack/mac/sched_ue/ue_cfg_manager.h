/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_UE_CFG_MANAGER_H
#define SRSRAN_UE_CFG_MANAGER_H

#include "../sched_nr_cfg.h"

namespace srsenb {
namespace sched_nr_impl {

using ue_cc_cfg_list = srsran::bounded_vector<sched_nr_ue_cc_cfg_t, SCHED_NR_MAX_CARRIERS>;

struct ue_cfg_manager {
  uint32_t                                       maxharq_tx = 4;
  ue_cc_cfg_list                                 carriers;
  std::array<mac_lc_ch_cfg_t, SCHED_NR_MAX_LCID> ue_bearers = {};
  srsran::phy_cfg_nr_t                           phy_cfg    = {};

  explicit ue_cfg_manager(uint32_t enb_cc_idx = 0);
  explicit ue_cfg_manager(const sched_nr_ue_cfg_t& cfg_req);
  int apply_config_request(const sched_nr_ue_cfg_t& cfg_req);
};

/// Semi-static configuration of a UE for a given CC.
class ue_carrier_params_t
{
public:
  ue_carrier_params_t() = default;
  explicit ue_carrier_params_t(uint16_t rnti, const bwp_params_t& active_bwp_cfg, const ue_cfg_manager& uecfg_);

  uint16_t rnti = SRSRAN_INVALID_RNTI;
  uint32_t cc   = SRSRAN_MAX_CARRIERS;

  const ue_cfg_manager&       ue_cfg() const { return *cfg_; }
  const srsran::phy_cfg_nr_t& phy() const { return cfg_->phy_cfg; }
  const bwp_params_t&         active_bwp() const { return *bwp_cfg; }

  /// Get SearchSpace based on SearchSpaceId
  const srsran_search_space_t* get_ss(uint32_t ss_id) const
  {
    if (phy().pdcch.search_space_present[ss_id]) {
      // UE-dedicated SearchSpace
      return &bwp_cfg->cfg.pdcch.search_space[ss_id];
    }
    return nullptr;
  }

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

  const srsran_dci_cfg_nr_t& get_dci_cfg() const { return cached_dci_cfg; }

  int find_ss_id(srsran_dci_format_nr_t dci_fmt) const;

private:
  const ue_cfg_manager* cfg_    = nullptr;
  const bwp_params_t*   bwp_cfg = nullptr;

  // derived
  std::vector<bwp_cce_pos_list>                              cce_positions_list;
  std::array<uint32_t, SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE> ss_id_to_cce_idx;
  srsran_dci_cfg_nr_t                                        cached_dci_cfg;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_UE_CFG_MANAGER_H
