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

#ifndef SRSRAN_CELL_ASN1_CONFIG_H
#define SRSRAN_CELL_ASN1_CONFIG_H

#include "rrc_nr_config.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/common_nr.h"

namespace srsenb {

using rlc_bearer_list_t = asn1::rrc_nr::cell_group_cfg_s::rlc_bearer_to_add_mod_list_l_;

// PHY helpers
void set_search_space_from_phy_cfg(const srsran_search_space_t& ss, asn1::rrc_nr::search_space_s& out);

// NSA helpers
int fill_sp_cell_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, asn1::rrc_nr::sp_cell_cfg_s& sp_cell);

// SA helpers
int fill_master_cell_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, asn1::rrc_nr::cell_group_cfg_s& out);

int fill_mib_from_enb_cfg(const rrc_cell_cfg_nr_t& cell_cfg, asn1::rrc_nr::mib_s& mib);
int fill_sib1_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, asn1::rrc_nr::sib1_s& sib1);

/**
 * Based on the previous and new radio bearer config, generate ASN1 diff
 * @return if a change was detected
 */
bool compute_diff_radio_bearer_cfg(const rrc_nr_cfg_t&                     cfg,
                                   const asn1::rrc_nr::radio_bearer_cfg_s& prev_bearers,
                                   const asn1::rrc_nr::radio_bearer_cfg_s& next_bearers,
                                   asn1::rrc_nr::radio_bearer_cfg_s&       diff);

/// Apply radioBearerConfig updates to CellGroupConfig
void fill_cellgroup_with_radio_bearer_cfg(const rrc_nr_cfg_t&                     cfg,
                                          const asn1::rrc_nr::radio_bearer_cfg_s& bearers,
                                          asn1::rrc_nr::cell_group_cfg_s&         out);

} // namespace srsenb

#endif // SRSRAN_CELL_ASN1_CONFIG_H
