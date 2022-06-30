/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "sched_nr_interface_utils.h"
#include "sched_nr_rb.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsran/adt/optional_array.h"

namespace srsenb {

static const size_t MAX_NOF_AGGR_LEVELS = 5;

namespace sched_nr_impl {

constexpr static size_t MAX_GRANTS = sched_nr_interface::MAX_GRANTS;

using pdcch_dl_t         = mac_interface_phy_nr::pdcch_dl_t;
using pdcch_ul_t         = mac_interface_phy_nr::pdcch_ul_t;
using pdsch_t            = mac_interface_phy_nr::pdsch_t;
using pusch_t            = mac_interface_phy_nr::pusch_t;
using pucch_t            = mac_interface_phy_nr::pucch_t;
using pdcch_dl_list_t    = srsran::bounded_vector<pdcch_dl_t, MAX_GRANTS>;
using pdcch_ul_list_t    = srsran::bounded_vector<pdcch_ul_t, MAX_GRANTS>;
using pdsch_list_t       = srsran::bounded_vector<pdsch_t, MAX_GRANTS>;
using pucch_list_t       = srsran::bounded_vector<pucch_t, MAX_GRANTS>;
using pusch_list_t       = srsran::bounded_vector<pusch_t, MAX_GRANTS>;
using nzp_csi_rs_list    = srsran::bounded_vector<srsran_csi_rs_nzp_resource_t, mac_interface_phy_nr::MAX_NZP_CSI_RS>;
using ssb_t              = mac_interface_phy_nr::ssb_t;
using ssb_list           = srsran::bounded_vector<ssb_t, mac_interface_phy_nr::MAX_SSB>;
using sched_args_t       = sched_nr_interface::sched_args_t;
using bwp_cfg_t          = sched_nr_bwp_cfg_t;
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

struct cell_config_manager;

/// Structure that extends the sched_nr_interface::bwp_cfg_t passed by upper layers with other
/// derived BWP-specific params
struct bwp_params_t {
  const uint32_t             bwp_id;
  const uint32_t             cc;
  const bwp_cfg_t            cfg;
  const cell_config_manager& cell_cfg;
  const sched_args_t&        sched_cfg;
  sched_nr_bwp_cfg_t         bwp_cfg;

  // derived params
  srslog::basic_logger& logger;
  uint32_t              P;
  uint32_t              N_rbg;
  uint32_t              nof_prb;

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

  srsran::optional_vector<bwp_cce_pos_list> common_cce_list;

  bwp_params_t(const cell_config_manager& cell, uint32_t bwp_id, const sched_nr_bwp_cfg_t& bwp_cfg);

  prb_interval  coreset_prb_range(uint32_t cs_id) const { return coresets[cs_id].prb_limits; }
  prb_interval  dci_fmt_1_0_prb_lims(uint32_t cs_id) const { return coresets[cs_id].dci_1_0_prb_limits; }
  bwp_rb_bitmap dci_fmt_1_0_excluded_prbs(uint32_t cs_id) const { return coresets[cs_id].usable_common_ss_prb_mask; }

  const srsran_search_space_t* get_ss(uint32_t ss_id) const
  {
    return cfg.pdcch.search_space_present[ss_id] ? &cfg.pdcch.search_space[ss_id] : nullptr;
  }

private:
  bwp_rb_bitmap cached_empty_prb_mask;
  struct coreset_cached_params {
    prb_interval  prb_limits;
    prb_interval  dci_1_0_prb_limits; /// See TS 38.214, section 5.1.2.2
    bwp_rb_bitmap usable_common_ss_prb_mask;
  };
  srsran::optional_vector<coreset_cached_params> coresets;
};

/// Structure packing a single cell config params, and sched args
struct cell_config_manager {
  const uint32_t                                    cc;
  srsran_carrier_nr_t                               carrier = {};
  srsran_mib_nr_t                                   mib;
  srsran::phy_cfg_nr_t::ssb_cfg_t                   ssb = {};
  std::vector<bwp_params_t>                         bwps; // idx0 for BWP-common
  std::vector<sched_nr_cell_cfg_sib_t>              sibs;
  asn1::copy_ptr<asn1::rrc_nr::dl_cfg_common_sib_s> dl_cfg_common;
  asn1::copy_ptr<asn1::rrc_nr::ul_cfg_common_sib_s> ul_cfg_common;
  srsran_duplex_config_nr_t                         duplex = {};
  const sched_args_t&                               sched_args;
  const srsran::phy_cfg_nr_t                        default_ue_phy_cfg;

  cell_config_manager(uint32_t cc_, const sched_nr_cell_cfg_t& cell, const sched_args_t& sched_args_);

  uint32_t nof_prb() const { return carrier.nof_prb; }
};

/// Structure packing both the sched args and all gNB NR cell configurations
struct sched_params_t {
  sched_args_t                     sched_cfg;
  std::vector<cell_config_manager> cells;

  sched_params_t() = default;
  explicit sched_params_t(const sched_args_t& sched_cfg_);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_CFG_H
