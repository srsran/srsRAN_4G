/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsenb/hdr/phy/phy_ue_db.h"

using namespace srsenb;

void phy_ue_db::init(stack_interface_phy_lte*   stack_ptr,
                     const phy_args_t&          phy_args_,
                     const phy_cell_cfg_list_t& cell_cfg_list_)
{
  stack         = stack_ptr;
  phy_args      = &phy_args_;
  cell_cfg_list = &cell_cfg_list_;
}

inline void phy_ue_db::_add_rnti(uint16_t rnti)
{
  // Private function not mutexed

  // Assert RNTI does NOT exist
  if (ue_db.count(rnti)) {
    return;
  }

  // Create new UE by accesing it
  ue_db[rnti].cell_info[0] = {};

  // Get UE
  common_ue& ue = ue_db[rnti];

  // Load default values to PCell
  ue.cell_info[0].phy_cfg.set_defaults();

  // Set constant configuration fields
  _set_common_config_rnti(rnti, ue.cell_info[0].phy_cfg);

  // Configure as PCell
  ue.cell_info[0].state = cell_state_primary;

  // Iterate all pending ACK
  for (uint32_t tti = 0; tti < TTIMOD_SZ; tti++) {
    _clear_tti_pending_rnti(tti, rnti);
  }
}

inline void phy_ue_db::_clear_tti_pending_rnti(uint32_t tti, uint16_t rnti)
{
  // Private function not mutexed, no need to assert RNTI or TTI

  // Get UE
  common_ue& ue = ue_db[rnti];

  srslte_pdsch_ack_t& pdsch_ack = ue.pdsch_ack[tti];

  // Reset ACK information
  pdsch_ack = {};

  uint32_t nof_active_cc = 0;
  for (auto& cell_info : ue.cell_info) {
    if (cell_info.state == cell_state_primary or cell_info.state == cell_state_secondary_active) {
      nof_active_cc++;
    }
  }

  // Copy essentials. It is assumed the PUCCH parameters are the same for all carriers
  pdsch_ack.transmission_mode      = ue.cell_info[0].phy_cfg.dl_cfg.tm;
  pdsch_ack.nof_cc                 = nof_active_cc;
  pdsch_ack.ack_nack_feedback_mode = ue.cell_info[0].phy_cfg.ul_cfg.pucch.ack_nack_feedback_mode;
  pdsch_ack.simul_cqi_ack          = ue.cell_info[0].phy_cfg.ul_cfg.pucch.simul_cqi_ack;
}

inline void phy_ue_db::_set_common_config_rnti(uint16_t rnti, srslte::phy_cfg_t& phy_cfg) const
{
  // Set common parameters
  phy_cfg.dl_cfg.pdsch.rnti                          = rnti;
  phy_cfg.ul_cfg.pucch.rnti                          = rnti;
  phy_cfg.ul_cfg.pusch.rnti                          = rnti;
  phy_cfg.ul_cfg.pusch.meas_time_en                  = true;
  phy_cfg.ul_cfg.pusch.meas_epre_en                  = phy_args->pusch_meas_epre;
  phy_cfg.ul_cfg.pusch.meas_ta_en                    = phy_args->pusch_meas_ta;
  phy_cfg.ul_cfg.pusch.meas_evm_en                   = phy_args->pusch_meas_evm;
  phy_cfg.ul_cfg.pusch.max_nof_iterations            = phy_args->pusch_max_its;
  phy_cfg.ul_cfg.pucch.threshold_format1             = SRSLTE_PUCCH_DEFAULT_THRESHOLD_FORMAT1;
  phy_cfg.ul_cfg.pucch.threshold_data_valid_format1a = SRSLTE_PUCCH_DEFAULT_THRESHOLD_FORMAT1A;
  phy_cfg.ul_cfg.pucch.threshold_data_valid_format2  = SRSLTE_PUCCH_DEFAULT_THRESHOLD_FORMAT2;
  phy_cfg.ul_cfg.pucch.threshold_data_valid_format3  = SRSLTE_PUCCH_DEFAULT_THRESHOLD_FORMAT3;
  phy_cfg.ul_cfg.pucch.threshold_dmrs_detection      = SRSLTE_PUCCH_DEFAULT_THRESHOLD_DMRS;
  phy_cfg.ul_cfg.pucch.meas_ta_en                    = phy_args->pucch_meas_ta;
}

inline uint32_t phy_ue_db::_get_ue_cc_idx(uint16_t rnti, uint32_t enb_cc_idx) const
{
  uint32_t         ue_cc_idx = 0;
  const common_ue& ue        = ue_db.at(rnti);

  for (; ue_cc_idx < SRSLTE_MAX_CARRIERS; ue_cc_idx++) {
    const cell_info_t& scell_info = ue.cell_info[ue_cc_idx];
    if (scell_info.enb_cc_idx == enb_cc_idx and
        (scell_info.state == cell_state_primary or scell_info.state == cell_state_secondary_active)) {
      return ue_cc_idx;
    }
  }

  return ue_cc_idx;
}

uint32_t phy_ue_db::_get_uci_enb_cc_idx(uint32_t tti, uint16_t rnti) const
{
  // Find the lowest index available PUSCH grant
  for (const cell_info_t& cell_info : ue_db.at(rnti).cell_info) {
    if (cell_info.is_grant_available[tti]) {
      return cell_info.enb_cc_idx;
    }
  }

  return (uint32_t)cell_cfg_list->size();
}

inline int phy_ue_db::_assert_rnti(uint16_t rnti) const
{
  if (not ue_db.count(rnti)) {
    ERROR("Trying to access RNTI 0x%X, it does not exist.\n", rnti);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

inline int phy_ue_db::_assert_enb_cc(uint16_t rnti, uint32_t enb_cc_idx) const
{
  // Assert RNTI exist
  if (_assert_rnti(rnti) != SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Check Component Carrier is part of UE SCell map
  if (_get_ue_cc_idx(rnti, enb_cc_idx) == SRSLTE_MAX_CARRIERS) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

inline int phy_ue_db::_assert_enb_pcell(uint16_t rnti, uint32_t enb_cc_idx) const
{
  if (_assert_enb_cc(rnti, enb_cc_idx) != SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Check cell is PCell
  const cell_info_t& cell_info = ue_db.at(rnti).cell_info[_get_ue_cc_idx(rnti, enb_cc_idx)];
  if (cell_info.state != cell_state_primary) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

inline int phy_ue_db::_assert_ue_cc(uint16_t rnti, uint32_t ue_cc_idx) const
{
  if (_assert_rnti(rnti) != SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Check the cell index is in range
  if (ue_cc_idx >= SRSLTE_MAX_CARRIERS) {
    return SRSLTE_ERROR;
  }

  const cell_info_t& cell_info = ue_db.at(rnti).cell_info.at(ue_cc_idx);
  if (cell_info.state == cell_state_none) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

inline int phy_ue_db::_assert_active_enb_cc(uint16_t rnti, uint32_t enb_cc_idx) const
{
  if (_assert_enb_cc(rnti, enb_cc_idx) != SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Check SCell is active, ignore PCell state
  const cell_info_t& cell_info = ue_db.at(rnti).cell_info[_get_ue_cc_idx(rnti, enb_cc_idx)];
  if (cell_info.state != cell_state_primary and cell_info.state != cell_state_secondary_active) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

inline int phy_ue_db::_assert_stack() const
{
  if (stack == nullptr) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

inline int phy_ue_db::_assert_cell_list_cfg() const
{
  if (cell_cfg_list == nullptr) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

inline srslte::phy_cfg_t phy_ue_db::_get_rnti_config(uint16_t rnti, uint32_t enb_cc_idx, bool stashed) const
{
  srslte::phy_cfg_t default_cfg = {};
  default_cfg.set_defaults();
  default_cfg.dl_cfg.pdsch.rnti = rnti;
  default_cfg.ul_cfg.pucch.rnti = rnti;
  default_cfg.ul_cfg.pusch.rnti = rnti;

  // Use default configuration for non-user C-RNTI
  if (not SRSLTE_RNTI_ISUSER(rnti)) {
    return default_cfg;
  }

  // Make sure the C-RNTI exists and the cell/carrier is configured
  if (_assert_enb_cc(rnti, enb_cc_idx) != SRSLTE_SUCCESS) {
    ERROR("Trying to access cell/carrier %d in RNTI 0x%X. It is not active.\n", enb_cc_idx, rnti);
    return default_cfg;
  }

  uint32_t ue_cc_idx = _get_ue_cc_idx(rnti, enb_cc_idx);

  // Return Stashed configuration if PCell and stashed is true
  if (ue_cc_idx == 0 and stashed) {
    return ue_db.at(rnti).pcell_cfg_stash;
  }

  // Otherwise return current configuration
  return ue_db.at(rnti).cell_info.at(ue_cc_idx).phy_cfg;
}

void phy_ue_db::clear_tti_pending_ack(uint32_t tti)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Iterate all UEs
  for (auto& iter : ue_db) {
    _clear_tti_pending_rnti(TTIMOD(tti), iter.first);
  }
}

void phy_ue_db::addmod_rnti(uint16_t rnti, const phy_interface_rrc_lte::phy_rrc_cfg_list_t& phy_cfg_list)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Create new user if did not exist
  if (ue_db.count(rnti) == 0) {
    _add_rnti(rnti);
  }

  // Get UE by reference
  common_ue& ue = ue_db[rnti];

  // Number of configured secondary serving cells
  uint32_t nof_configured_scell = 0;

  // Iterate PHY RRC configuration for each UE cell/carrier
  uint32_t nof_cc = SRSLTE_MIN(phy_cfg_list.size(), SRSLTE_MAX_CARRIERS);
  for (uint32_t ue_cc_idx = 0; ue_cc_idx < nof_cc; ue_cc_idx++) {
    const phy_interface_rrc_lte::phy_rrc_cfg_t& phy_rrc_dedicated = phy_cfg_list[ue_cc_idx];

    // Configured, add/modify entry in the cell_info map
    cell_info_t& cell_info = ue.cell_info[ue_cc_idx];

    // Configure PHY
    if (cell_info.state == cell_state_primary) {
      // If primary serving cell's eNb cell/carrier index changed, it applies default current config
      if (cell_info.enb_cc_idx != phy_rrc_dedicated.enb_cc_idx) {
        cell_info.phy_cfg.set_defaults();
        _set_common_config_rnti(rnti, cell_info.phy_cfg);
      }

      // Apply primmary serving cell configuration in stash
      ue.pcell_cfg_stash = phy_rrc_dedicated.phy_cfg;
      _set_common_config_rnti(rnti, ue.pcell_cfg_stash);
    } else if (phy_rrc_dedicated.configured) {
      // Only set to inactive if cell is not already configured
      if (cell_info.state == cell_state_t::cell_state_none) {
        cell_info.phy_cfg = phy_rrc_dedicated.phy_cfg;
        _set_common_config_rnti(rnti, cell_info.phy_cfg);

        // Set Cell state, all inactive by default
        cell_info.state = cell_state_secondary_inactive;
      }
      // Count Serving cell
      nof_configured_scell++;
    } else {
      // Cell without configuration (except PCell)
      cell_info.state = cell_state_none;
    }

    // Set serving cell index
    cell_info.enb_cc_idx = phy_rrc_dedicated.enb_cc_idx;
  }

  // Disable the rest of potential serving cells
  for (uint32_t i = nof_cc; i < SRSLTE_MAX_CARRIERS; i++) {
    ue.cell_info[i].state = cell_state_none;
  }

  // Enable/Disable extended CSI field in DCI according to 3GPP 36.212 R10 5.3.3.1.1 Format 0
  for (uint32_t ue_cc_idx = 0; ue_cc_idx < nof_cc; ue_cc_idx++) {
    if (ue.cell_info[ue_cc_idx].state == cell_state_primary) {
      // The primary cell applies changes in the stashed config
      ue.pcell_cfg_stash.dl_cfg.dci.multiple_csi_request_enabled = (nof_configured_scell > 0);
    } else {
      // The rest apply changes directly
      ue.cell_info[ue_cc_idx].phy_cfg.dl_cfg.dci.multiple_csi_request_enabled = (nof_configured_scell > 0);
    }
  }

  // Load new UL configuration
  ue.cell_info[0].phy_cfg.ul_cfg = ue.pcell_cfg_stash.ul_cfg;
}

void phy_ue_db::rem_rnti(uint16_t rnti)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (ue_db.count(rnti) != 0) {
    ue_db.erase(rnti);
  }
}

void phy_ue_db::complete_config(uint16_t rnti)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Makes sure the RNTI exists
  if (_assert_rnti(rnti) != SRSLTE_SUCCESS) {
    return;
  }

  // Apply stashed configuration
  ue_db[rnti].cell_info[0].phy_cfg = ue_db[rnti].pcell_cfg_stash;
}

void phy_ue_db::activate_deactivate_scell(uint16_t rnti, uint32_t ue_cc_idx, bool activate)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Assert RNTI and SCell are valid
  if (_assert_ue_cc(rnti, ue_cc_idx) != SRSLTE_SUCCESS) {
    return;
  }

  cell_info_t& cell_info = ue_db[rnti].cell_info[ue_cc_idx];

  // If scell is default only complain
  if (activate and cell_info.state == cell_state_none) {
    ERROR("RNTI 0x%X SCell %d has received an activation MAC command but it was not configured\n", rnti, ue_cc_idx);
    return;
  }
  // Set scell state
  cell_info.state = (activate) ? cell_state_secondary_active : cell_state_secondary_inactive;
}

bool phy_ue_db::is_pcell(uint16_t rnti, uint32_t enb_cc_idx) const
{
  std::lock_guard<std::mutex> lock(mutex);
  return _assert_enb_pcell(rnti, enb_cc_idx) == SRSLTE_SUCCESS;
}

srslte_dl_cfg_t phy_ue_db::get_dl_config(uint16_t rnti, uint32_t enb_cc_idx) const
{
  std::lock_guard<std::mutex> lock(mutex);
  return _get_rnti_config(rnti, enb_cc_idx, false).dl_cfg;
}

srslte_dci_cfg_t phy_ue_db::get_dci_dl_config(uint16_t rnti, uint32_t enb_cc_idx) const
{
  std::lock_guard<std::mutex> lock(mutex);
  return _get_rnti_config(rnti, enb_cc_idx, false).dl_cfg.dci;
}

srslte_ul_cfg_t phy_ue_db::get_ul_config(uint16_t rnti, uint32_t enb_cc_idx) const
{
  std::lock_guard<std::mutex> lock(mutex);
  return _get_rnti_config(rnti, enb_cc_idx, false).ul_cfg;
}

srslte_dci_cfg_t phy_ue_db::get_dci_ul_config(uint16_t rnti, uint32_t enb_cc_idx) const
{
  std::lock_guard<std::mutex> lock(mutex);
  return _get_rnti_config(rnti, enb_cc_idx, true).dl_cfg.dci;
}

void phy_ue_db::set_ack_pending(uint32_t tti, uint32_t enb_cc_idx, const srslte_dci_dl_t& dci)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Assert rnti and cell exits and it is active
  if (_assert_active_enb_cc(dci.rnti, enb_cc_idx) != SRSLTE_SUCCESS) {
    return;
  }

  common_ue& ue        = ue_db[dci.rnti];
  uint32_t   ue_cc_idx = _get_ue_cc_idx(dci.rnti, enb_cc_idx);

  srslte_pdsch_ack_cc_t& pdsch_ack_cc = ue.pdsch_ack[tti].cc[ue_cc_idx];
  pdsch_ack_cc.M                      = 1; ///< Hardcoded for FDD

  // Fill PDSCH ACK information
  srslte_pdsch_ack_m_t& pdsch_ack_m  = pdsch_ack_cc.m[0]; ///< Assume FDD only
  pdsch_ack_m.present                = true;
  pdsch_ack_m.resource.grant_cc_idx  = ue_cc_idx; ///< Assumes no cross-carrier scheduling
  pdsch_ack_m.resource.v_dai_dl      = 0;         ///< Ignore for FDD
  pdsch_ack_m.resource.n_cce         = dci.location.ncce;
  pdsch_ack_m.resource.tpc_for_pucch = dci.tpc_pucch;

  // Set TB info
  for (uint32_t tb_idx = 0; tb_idx < SRSLTE_MAX_CODEWORDS; tb_idx++) {
    // Count only if the TB is enabled and the TB index is valid for the DCI format
    if (SRSLTE_DCI_IS_TB_EN(dci.tb[tb_idx]) and tb_idx < srslte_dci_format_max_tb(dci.format)) {
      pdsch_ack_m.value[tb_idx] = 1;
      pdsch_ack_m.k++;
    } else {
      pdsch_ack_m.value[tb_idx] = 2;
    }
  }
}

bool phy_ue_db::fill_uci_cfg(uint32_t          tti,
                             uint32_t          enb_cc_idx,
                             uint16_t          rnti,
                             bool              aperiodic_cqi_request,
                             bool              is_pusch_available,
                             srslte_uci_cfg_t& uci_cfg)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Reset UCI CFG, avoid returning carrying cached information
  uci_cfg = {};

  // Assert Cell List configuration
  if (_assert_cell_list_cfg() != SRSLTE_SUCCESS) {
    return false;
  }

  // Assert eNb Cell/Carrier for the given RNTI
  if (_assert_active_enb_cc(rnti, enb_cc_idx) != SRSLTE_SUCCESS) {
    return false;
  }

  // Get the eNb cell/carrier index with lowest serving cell index (ue_cc_idx) that has an available grant.
  uint32_t uci_enb_cc_id         = _get_uci_enb_cc_idx(tti, rnti);
  bool     pusch_grant_available = (uci_enb_cc_id < (uint32_t)cell_cfg_list->size());

  // There is a PUSCH grant available for the provided RNTI in at least one serving cell and this call is for PUCCH
  if (pusch_grant_available and not is_pusch_available) {
    return false;
  }

  // There is a PUSCH grant and enb_cc_idx with lowest ue_cc_idx with a grant
  if (pusch_grant_available and uci_enb_cc_id != enb_cc_idx) {
    return false;
  }

  // No PUSCH grant for this TTI and cell and no enb_cc_idx is not the PCell
  if (not pusch_grant_available and _get_ue_cc_idx(rnti, enb_cc_idx) != 0) {
    return false;
  }

  common_ue&               ue           = ue_db.at(rnti);
  const srslte::phy_cfg_t& pcell_cfg    = ue.cell_info[0].phy_cfg;
  bool                     uci_required = false;

  const cell_info_t&   pcell_info = ue.cell_info[0];
  const srslte_cell_t& pcell      = cell_cfg_list->at(pcell_info.enb_cc_idx).cell;

  // Check if SR opportunity (will only be used in PUCCH)
  uci_cfg.is_scheduling_request_tti = (srslte_ue_ul_sr_send_tti(&pcell_cfg.ul_cfg.pucch, tti) == 1);
  uci_required |= uci_cfg.is_scheduling_request_tti;

  // Get pending CQI reports for this TTI, stops at first CC reporting
  bool periodic_cqi_required = false;
  for (uint32_t cell_idx = 0; cell_idx < SRSLTE_MAX_CARRIERS and not periodic_cqi_required; cell_idx++) {
    const cell_info_t&     cell_info = ue.cell_info[cell_idx];
    const srslte_dl_cfg_t& dl_cfg    = cell_info.phy_cfg.dl_cfg;

    // Check report for primary and active and inactive cells. CQI reports start after RRC configures the carrier
    if (cell_info.state != cell_state_none) {
      const srslte_cell_t& cell = cell_cfg_list->at(cell_info.enb_cc_idx).cell;

      // Check if CQI report is required
      periodic_cqi_required = srslte_enb_dl_gen_cqi_periodic(&cell, &dl_cfg, tti, cell_info.last_ri, &uci_cfg.cqi);

      // Save SCell index for using it after
      uci_cfg.cqi.scell_index = cell_idx;
    }
  }
  uci_required |= periodic_cqi_required;

  // If no periodic CQI report required, check aperiodic reporting
  if ((not periodic_cqi_required) and aperiodic_cqi_request) {
    // Aperiodic only supported for PCell
    const srslte_dl_cfg_t& dl_cfg = pcell_info.phy_cfg.dl_cfg;

    uci_required = srslte_enb_dl_gen_cqi_aperiodic(&pcell, &dl_cfg, pcell_info.last_ri, &uci_cfg.cqi);
  }

  // Get pending ACKs from PDSCH
  srslte_dl_sf_cfg_t dl_sf_cfg  = {};
  dl_sf_cfg.tti                 = tti;
  srslte_pdsch_ack_t& pdsch_ack = ue.pdsch_ack[tti];
  pdsch_ack.is_pusch_available  = is_pusch_available;
  srslte_enb_dl_gen_ack(&pcell, &dl_sf_cfg, &pdsch_ack, &uci_cfg);
  uci_required |= (srslte_uci_cfg_total_ack(&uci_cfg) > 0);

  // Return whether UCI needs to be decoded
  return uci_required;
}

void phy_ue_db::send_uci_data(uint32_t                  tti,
                              uint16_t                  rnti,
                              uint32_t                  enb_cc_idx,
                              const srslte_uci_cfg_t&   uci_cfg,
                              const srslte_uci_value_t& uci_value)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Assert UE RNTI database entry and eNb cell/carrier must be active
  if (_assert_active_enb_cc(rnti, enb_cc_idx) != SRSLTE_SUCCESS) {
    return;
  }

  // Assert Stack
  if (_assert_stack() != SRSLTE_SUCCESS) {
    return;
  }

  // Notify SR
  if (uci_cfg.is_scheduling_request_tti && uci_value.scheduling_request) {
    stack->sr_detected(tti, rnti);
  }

  // Get UE
  common_ue& ue = ue_db.at(rnti);

  // Get ACK info
  srslte_pdsch_ack_t& pdsch_ack = ue.pdsch_ack[tti];
  srslte_enb_dl_get_ack(&cell_cfg_list->at(ue.cell_info[0].enb_cc_idx).cell, &uci_cfg, &uci_value, &pdsch_ack);

  // Iterate over the ACK information
  for (uint32_t ue_cc_idx = 0; ue_cc_idx < SRSLTE_MAX_CARRIERS; ue_cc_idx++) {
    const srslte_pdsch_ack_cc_t& pdsch_ack_cc = pdsch_ack.cc[ue_cc_idx];
    for (uint32_t m = 0; m < pdsch_ack_cc.M; m++) {
      if (pdsch_ack_cc.m[m].present) {
        for (uint32_t tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
          if (pdsch_ack_cc.m[m].value[tb] != 2) {
            stack->ack_info(tti, rnti, ue.cell_info[ue_cc_idx].enb_cc_idx, tb, pdsch_ack_cc.m[m].value[tb] == 1);
          }
        }
      }
    }
  }

  // Assert the SCell exists and it is active
  if (_assert_ue_cc(rnti, uci_cfg.cqi.scell_index) != SRSLTE_SUCCESS) {
    return;
  }

  // Get CQI carrier index
  cell_info_t& cqi_scell_info = ue_db.at(rnti).cell_info[uci_cfg.cqi.scell_index];
  uint32_t     cqi_cc_idx     = cqi_scell_info.enb_cc_idx;

  // Notify CQI only if CRC is valid
  if (uci_value.cqi.data_crc) {
    // Channel quality indicator itself
    if (uci_cfg.cqi.data_enable) {
      uint8_t cqi_value = 0;
      switch (uci_cfg.cqi.type) {
        case SRSLTE_CQI_TYPE_WIDEBAND:
          cqi_value = uci_value.cqi.wideband.wideband_cqi;
          break;
        case SRSLTE_CQI_TYPE_SUBBAND:
          cqi_value = uci_value.cqi.subband.subband_cqi;
          break;
        case SRSLTE_CQI_TYPE_SUBBAND_HL:
          cqi_value = uci_value.cqi.subband_hl.wideband_cqi_cw0;
          break;
        case SRSLTE_CQI_TYPE_SUBBAND_UE:
          cqi_value = uci_value.cqi.subband_ue.wideband_cqi;
          break;
      }
      stack->cqi_info(tti, rnti, cqi_cc_idx, cqi_value);
    }

    // Precoding Matrix indicator (TM4)
    if (uci_cfg.cqi.pmi_present) {
      uint8_t pmi_value = 0;
      switch (uci_cfg.cqi.type) {
        case SRSLTE_CQI_TYPE_WIDEBAND:
          pmi_value = uci_value.cqi.wideband.pmi;
          break;
        case SRSLTE_CQI_TYPE_SUBBAND_HL:
          pmi_value = uci_value.cqi.subband_hl.pmi;
          break;
        default:
          ERROR("CQI type=%d not implemented for PMI\n", uci_cfg.cqi.type);
          break;
      }
      stack->pmi_info(tti, rnti, cqi_cc_idx, pmi_value);
    }
  }

  // Rank indicator (TM3 and TM4)
  if (uci_cfg.cqi.ri_len) {
    stack->ri_info(tti, rnti, cqi_cc_idx, uci_value.ri);
    cqi_scell_info.last_ri = uci_value.ri;
  }
}

void phy_ue_db::set_last_ul_tb(uint16_t rnti, uint32_t enb_cc_idx, uint32_t pid, srslte_ra_tb_t tb)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Assert UE DB entry
  if (_assert_active_enb_cc(rnti, enb_cc_idx) != SRSLTE_SUCCESS) {
    return;
  }

  // Save resource allocation
  ue_db.at(rnti).cell_info[_get_ue_cc_idx(rnti, enb_cc_idx)].last_tb[pid] = tb;
}

srslte_ra_tb_t phy_ue_db::get_last_ul_tb(uint16_t rnti, uint32_t enb_cc_idx, uint32_t pid) const
{
  std::lock_guard<std::mutex> lock(mutex);

  // Assert UE DB entry
  if (_assert_active_enb_cc(rnti, enb_cc_idx) != SRSLTE_SUCCESS) {
    return {};
  }

  // Returns the latest stored UL transmission grant
  return ue_db.at(rnti).cell_info[_get_ue_cc_idx(rnti, enb_cc_idx)].last_tb[pid];
}

void phy_ue_db::set_ul_grant_available(uint32_t tti, const stack_interface_phy_lte::ul_sched_list_t& ul_sched_list)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Reset all available grants flags for the given TTI
  for (auto& ue : ue_db) {
    for (cell_info_t& cell_info : ue.second.cell_info) {
      cell_info.is_grant_available[tti] = false;
    }
  }

  // For each eNb Cell/Carrier grant set a flag to the corresponding RNTI
  for (uint32_t enb_cc_idx = 0; enb_cc_idx < (uint32_t)ul_sched_list.size(); enb_cc_idx++) {
    const stack_interface_phy_lte::ul_sched_t& ul_sched = ul_sched_list[enb_cc_idx];
    for (uint32_t i = 0; i < ul_sched.nof_grants; i++) {
      const stack_interface_phy_lte::ul_sched_grant_t& ul_sched_grant = ul_sched.pusch[i];
      uint16_t                                         rnti           = ul_sched_grant.dci.rnti;
      // Check that eNb Cell/Carrier is active for the given RNTI
      if (_assert_active_enb_cc(rnti, enb_cc_idx) == SRSLTE_SUCCESS) {
        // Rise Grant available flag
        ue_db[rnti].cell_info[_get_ue_cc_idx(rnti, enb_cc_idx)].is_grant_available[tti] = true;
      }
    }
  }
}
