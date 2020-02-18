/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

inline void phy_ue_db::_add_rnti(uint16_t rnti)
{
  // Private function not mutexed

  // Assert RNTI does NOT exist
  if (ue_db.count(rnti)) {
    return;
  }

  // Create new UE by accesing it
  ue_db[rnti].scell_info[0] = {};

  // Get UE
  common_ue& ue = ue_db[rnti];

  // Load default values to PCell
  ue.scell_info[0].phy_cfg.set_defaults();

  // Set constant configuration fields
  _set_config_rnti(rnti);

  // PCell shall be active by default
  ue.scell_info[0].state = scell_state_active;

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
  for (auto& scell_info : ue.scell_info) {
    if (scell_info.state == scell_state_active) {
      nof_active_cc++;
    }
  }

  // Copy essentials
  pdsch_ack.transmission_mode      = ue.scell_info[0].phy_cfg.dl_cfg.tm;
  pdsch_ack.nof_cc                 = nof_active_cc;
  pdsch_ack.ack_nack_feedback_mode = ue.scell_info[0].phy_cfg.ul_cfg.pucch.ack_nack_feedback_mode;
  pdsch_ack.simul_cqi_ack          = ue.scell_info[0].phy_cfg.ul_cfg.pucch.simul_cqi_ack;
}

inline void phy_ue_db::_set_config_rnti(uint16_t rnti)
{
  // Private function not mutexed, no need to assert RNTI or TTI

  // Get UE
  common_ue& ue = ue_db[rnti];

  // Iterate all cells/carriers
  for (auto& scell_info : ue.scell_info) {
    scell_info.phy_cfg.dl_cfg.pdsch.rnti                          = rnti;
    scell_info.phy_cfg.ul_cfg.pucch.rnti                          = rnti;
    scell_info.phy_cfg.ul_cfg.pusch.rnti                          = rnti;
    scell_info.phy_cfg.ul_cfg.pucch.threshold_format1             = SRSLTE_PUCCH_DEFAULT_THRESHOLD_FORMAT1;
    scell_info.phy_cfg.ul_cfg.pucch.threshold_data_valid_format1a = SRSLTE_PUCCH_DEFAULT_THRESHOLD_FORMAT1A;
    scell_info.phy_cfg.ul_cfg.pucch.threshold_data_valid_format2  = SRSLTE_PUCCH_DEFAULT_THRESHOLD_FORMAT2;
  }
}

inline uint32_t phy_ue_db::_get_scell_idx(uint16_t rnti, uint32_t cc_idx) const
{
  uint32_t         scell_idx = 0;
  const common_ue& ue        = ue_db.at(rnti);

  for (scell_idx = 0; scell_idx < SRSLTE_MAX_CARRIERS; scell_idx++) {
    const scell_info_t& scell_info = ue.scell_info[scell_idx];
    if (scell_info.cc_idx == cc_idx && scell_info.state != scell_state_deactivated) {
      return scell_idx;
    }
  }

  return scell_idx;
}

void phy_ue_db::clear_tti_pending_ack(uint32_t tti)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Iterate all UEs
  for (auto& iter : ue_db) {
    _clear_tti_pending_rnti(TTIMOD(tti), iter.first);
  }
}

void phy_ue_db::addmod_rnti(uint16_t                                               rnti,
                            const phy_interface_rrc_lte::phy_rrc_dedicated_list_t& phy_rrc_dedicated_list)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Create new user if did not exist
  if (!ue_db.count(rnti)) {
    _add_rnti(rnti);
  }

  // Get UE by reference
  common_ue& ue = ue_db[rnti];

  // Iterate PHY RRC configuration for each cell/carrier
  for (uint32_t scell_idx = 0; scell_idx < phy_rrc_dedicated_list.size() && scell_idx < SRSLTE_MAX_CARRIERS;
       scell_idx++) {
    auto& phy_rrc_dedicated = phy_rrc_dedicated_list[scell_idx];
    // Configured, add/modify entry in the scell_info map
    auto& scell_info = ue.scell_info[scell_idx];

    if (phy_rrc_dedicated.configured) {
      // Set SCell information
      scell_info.cc_idx  = phy_rrc_dedicated.cc_idx;
      scell_info.phy_cfg = phy_rrc_dedicated.phy_cfg;

      // Set constant configuration fields
      _set_config_rnti(rnti);

      // Set SCell state, all deactivated by default except PCell
      scell_info.state = scell_idx == 0 ? scell_state_active : scell_state_deactivated;
    } else {
      // Cell without configuration shall be default
      scell_info.state = scell_state_default;
    }
  }

  // Iterate the rest of SCells
  for (uint32_t scell_idx = phy_rrc_dedicated_list.size(); scell_idx < SRSLTE_MAX_CARRIERS; scell_idx++) {
    // Set state of these to default
    ue.scell_info[scell_idx].state = scell_state_default;
  }
}

void phy_ue_db::rem_rnti(uint16_t rnti)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (ue_db.count(rnti)) {
    ue_db.erase(rnti);
  }
}

/**
 * UE Database Assert macros. These macros avoid repeating code for asserting RNTI, eNb cell/carrier index, SCell
 * indexes and so on.
 *
 * They are const friendly. All the methods they use of the attributes are const, so they do not modify any attribute.
 */
#define UE_DB_ASSERT_RNTI(RNTI, RET)                                                                                   \
  do {                                                                                                                 \
    if (not ue_db.count(RNTI)) {                                                                                       \
      /*ERROR("Trying to access RNTI x%x, it does not exist.\n", RNTI);*/                                              \
      return RET;                                                                                                      \
    }                                                                                                                  \
  } while (false)

#define UE_DB_ASSERT_CELL(RNTI, CC_IDX, RET)                                                                           \
  do {                                                                                                                 \
    /* Check if the UE exists */                                                                                       \
    UE_DB_ASSERT_RNTI(RNTI, RET);                                                                                      \
                                                                                                                       \
    /* Check Component Carrier is part of UE SCell map*/                                                               \
    if (_get_scell_idx(RNTI, CC_IDX) == SRSLTE_MAX_CARRIERS) {                                                         \
      ERROR("Trying to access cell/carrier index %d in RNTI x%x. It does not exist.\n", CC_IDX, RNTI);                 \
      return RET;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    /* Check SCell index is in range */                                                                                \
    const uint32_t scell_idx = _get_scell_idx(RNTI, CC_IDX);                                                           \
    if (scell_idx == SRSLTE_MAX_CARRIERS) {                                                                            \
      ERROR("Corrupted SCell index %d for RNTI x%x and cell/carrier index %d\n", scell_idx, RNTI, CC_IDX);             \
      return RET;                                                                                                      \
    }                                                                                                                  \
  } while (false)

#define UE_DB_ASSERT_ACTIVE_CELL(RNTI, CC_IDX, RET)                                                                    \
  do {                                                                                                                 \
    /* Assert RNTI exists and eNb cell/carrier is configured */                                                        \
    UE_DB_ASSERT_CELL(RNTI, CC_IDX, RET);                                                                              \
                                                                                                                       \
    /* Check SCell is active */                                                                                        \
    auto& scell_info = ue_db.at(RNTI).scell_info[_get_scell_idx(RNTI, CC_IDX)];                                        \
    if (scell_info.state != scell_state_active) {                                                                      \
      ERROR("Failed to assert active cell/carrier %d for RNTI x%x", CC_IDX, RNTI);                                     \
      return RET;                                                                                                      \
    }                                                                                                                  \
  } while (false)

#define UE_DB_ASSERT_PCELL(RNTI, CC_IDX, RET)                                                                          \
  do {                                                                                                                 \
    /* Assert RNTI exists and eNb cell/carrier is configured */                                                        \
    UE_DB_ASSERT_CELL(RNTI, CC_IDX, RET);                                                                              \
                                                                                                                       \
    /* CC_IDX is the RNTI PCell */                                                                                     \
    if (_get_scell_idx(RNTI, CC_IDX) != 0) {                                                                           \
      return RET;                                                                                                      \
    }                                                                                                                  \
  } while (false)

#define UE_DB_ASSERT_SCELL(RNTI, SCELL_IDX, RET)                                                                       \
  do {                                                                                                                 \
    /* Assert RNTI exists and eNb cell/carrier is configured */                                                        \
    UE_DB_ASSERT_RNTI(RNTI, RET);                                                                                      \
                                                                                                                       \
    /* Check SCell index is in range */                                                                                \
    if (SCELL_IDX >= SRSLTE_MAX_CARRIERS) {                                                                            \
      ERROR("Out-of-bounds SCell index %d for RNTI x%x.\n", SCELL_IDX, RNTI);                                          \
      return RET;                                                                                                      \
    }                                                                                                                  \
  } while (false)

#define UE_DB_ASSERT_ACTIVE_SCELL(RNTI, SCELL_IDX, RET)                                                                \
  do {                                                                                                                 \
    /* Assert RNTI exists and eNb cell/carrier is configured */                                                        \
    UE_DB_ASSERT_SCELL(RNTI, SCELL_IDX, RET);                                                                          \
                                                                                                                       \
    /* Check SCell is active */                                                                                        \
    auto& scell_info = ue_db.at(RNTI).scell_info[SCELL_IDX];                                                           \
    if (scell_info.state != scell_state_active) {                                                                      \
      ERROR("Failed to assert active SCell %d for RNTI x%x", SCELL_IDX, RNTI);                                         \
      return RET;                                                                                                      \
    }                                                                                                                  \
  } while (false)

#define UE_DB_ASSERT_STACK(RET)                                                                                        \
  do {                                                                                                                 \
    if (not stack) {                                                                                                   \
      return RET;                                                                                                      \
    }                                                                                                                  \
  } while (false)

#define UE_DB_ASSERT_CELL_LIST_CFG(RET)                                                                                \
  do {                                                                                                                 \
    if (not cell_cfg_list) {                                                                                           \
      return RET;                                                                                                      \
    }                                                                                                                  \
  } while (false)

void phy_ue_db::activate_deactivate_scell(uint16_t rnti, uint32_t scell_idx, bool activate)
{
  // Assert RNTI and SCell are valid
  UE_DB_ASSERT_SCELL(rnti, scell_idx, /* void */);

  auto& scell_info = ue_db[rnti].scell_info[scell_idx];

  // If scell is default only complain
  if (activate and scell_info.state == scell_state_default) {
    ERROR("RNTI x%x SCell %d has received an activation MAC command but it was not configured\n", rnti, scell_idx);
    return;
  }

  // Set scell state
  scell_info.state = (activate) ? scell_state_active : scell_state_deactivated;
}

srslte::phy_cfg_t phy_ue_db::get_config(uint16_t rnti, uint32_t cc_idx) const
{
  std::lock_guard<std::mutex> lock(mutex);

  UE_DB_ASSERT_ACTIVE_CELL(rnti, cc_idx, {});

  return ue_db.at(rnti).scell_info[_get_scell_idx(rnti, cc_idx)].phy_cfg;
}

void phy_ue_db::set_ack_pending(uint32_t tti, uint32_t cc_idx, const srslte_dci_dl_t& dci)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Assert rnti and cell exits and it is active
  UE_DB_ASSERT_ACTIVE_CELL(dci.rnti, cc_idx, /* void */);

  common_ue& ue        = ue_db[dci.rnti];
  uint32_t   scell_idx = _get_scell_idx(dci.rnti, cc_idx);

  srslte_pdsch_ack_cc_t& pdsch_ack_cc = ue.pdsch_ack[TTIMOD(tti)].cc[scell_idx];
  pdsch_ack_cc.M                      = 1; ///< Hardcoded for FDD

  // Fill PDSCH ACK information
  srslte_pdsch_ack_m_t& pdsch_ack_m  = pdsch_ack_cc.m[0]; ///< Assume FDD only
  pdsch_ack_m.present                = true;
  pdsch_ack_m.resource.grant_cc_idx  = cc_idx; ///< Assumes no cross-carrier scheduling
  pdsch_ack_m.resource.v_dai_dl      = 0;      ///< Ignore for FDD
  pdsch_ack_m.resource.n_cce         = dci.location.ncce;
  pdsch_ack_m.resource.tpc_for_pucch = dci.tpc_pucch;

  // Set TB info
  for (uint32_t i = 0; i < srslte_dci_format_max_tb(dci.format); i++) {
    if (SRSLTE_DCI_IS_TB_EN(dci.tb[i])) {
      pdsch_ack_m.value[i] = true;
      pdsch_ack_m.k++;
    }
  }
}

bool phy_ue_db::fill_uci_cfg(uint32_t          tti,
                             uint32_t          cc_idx,
                             uint16_t          rnti,
                             bool              aperiodic_cqi_request,
                             srslte_uci_cfg_t& uci_cfg) const
{
  std::lock_guard<std::mutex> lock(mutex);

  // Assert rnti and cell exits and it is active
  UE_DB_ASSERT_PCELL(rnti, cc_idx, false);

  // Assert Cell List configuration
  UE_DB_ASSERT_CELL_LIST_CFG(false);

  const auto& ue           = ue_db.at(rnti);
  const auto& pcell_cfg    = ue.scell_info[0].phy_cfg;
  bool        uci_required = false;

  uci_cfg = {};

  // Check if SR opportunity (will only be used in PUCCH)
  uci_cfg.is_scheduling_request_tti = (srslte_ue_ul_sr_send_tti(&pcell_cfg.ul_cfg.pucch, tti) == 1);
  uci_required |= uci_cfg.is_scheduling_request_tti;

  // Get pending CQI reports for this TTI, stops at first CC reporting
  bool periodic_cqi_required = false;
  for (uint32_t scell_idx = 0; scell_idx < SRSLTE_MAX_CARRIERS and not periodic_cqi_required; scell_idx++) {
    const scell_info_t&    scell_info = ue.scell_info[scell_idx];
    const srslte_dl_cfg_t& dl_cfg     = scell_info.phy_cfg.dl_cfg;

    if (scell_info.state == scell_state_active) {
      const srslte_cell_t& cell = cell_cfg_list->at(scell_info.cc_idx).cell;

      // Check if CQI report is required
      periodic_cqi_required = srslte_enb_dl_gen_cqi_periodic(&cell, &dl_cfg, tti, scell_info.last_ri, &uci_cfg.cqi);

      // Save SCell index for using it after
      uci_cfg.cqi.scell_index = scell_idx;
    }
  }
  uci_required |= periodic_cqi_required;

  // If no periodic CQI report required, check aperiodic reporting
  if ((not periodic_cqi_required) and aperiodic_cqi_request) {
    // Aperiodic only supported for PCell
    const scell_info_t&    pcell_info = ue.scell_info[0];
    const srslte_cell_t&   cell       = cell_cfg_list->at(pcell_info.cc_idx).cell;
    const srslte_dl_cfg_t& dl_cfg     = pcell_info.phy_cfg.dl_cfg;

    uci_required = srslte_enb_dl_gen_cqi_aperiodic(&cell, &dl_cfg, pcell_info.last_ri, &uci_cfg.cqi);
  }

  // Get pending ACKs from PDSCH
  srslte_dl_sf_cfg_t dl_sf_cfg = {};
  dl_sf_cfg.tti                = tti;
  const srslte_cell_t& cell    = cell_cfg_list->at(ue.scell_info[0].cc_idx).cell;
  srslte_enb_dl_gen_ack(&cell, &dl_sf_cfg, &ue.pdsch_ack[TTIMOD(tti)], &uci_cfg);
  uci_required |= (srslte_uci_cfg_total_ack(&uci_cfg) > 0);

  // Return whether UCI needs to be decoded
  return uci_required;
}

void phy_ue_db::send_uci_data(uint32_t                  tti,
                              uint16_t                  rnti,
                              uint32_t                  cc_idx,
                              const srslte_uci_cfg_t&   uci_cfg,
                              const srslte_uci_value_t& uci_value)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Assert UE RNTI database entry and eNb cell/carrier must be primary cell
  UE_DB_ASSERT_PCELL(rnti, cc_idx, /* void */);

  // Assert Stack
  UE_DB_ASSERT_STACK(/* void */);

  // Notify SR
  if (uci_cfg.is_scheduling_request_tti && uci_value.scheduling_request) {
    stack->sr_detected(tti, rnti);
  }

  // Get UE
  common_ue& ue = ue_db.at(rnti);

  // Get ACK info
  srslte_pdsch_ack_t& pdsch_ack = ue.pdsch_ack[TTIMOD(tti)];
  srslte_enb_dl_get_ack(&cell_cfg_list->at(ue.scell_info[0].cc_idx).cell, &uci_value, &pdsch_ack);

  // Iterate over the ACK information
  for (uint32_t scell_idx = 0; scell_idx < SRSLTE_MAX_CARRIERS; scell_idx++) {
    const srslte_pdsch_ack_cc_t& pdsch_ack_cc = pdsch_ack.cc[scell_idx];
    for (uint32_t m = 0; m < pdsch_ack_cc.M; m++) {
      if (pdsch_ack_cc.m[m].present) {
        for (uint32_t tb = 0; tb < pdsch_ack_cc.m[m].k; tb++) {
          stack->ack_info(tti, rnti, ue.scell_info[scell_idx].cc_idx, tb, pdsch_ack_cc.m[m].value[tb] == 1);
        }
      }
    }
  }

  // Assert the SCell exists and it is active
  UE_DB_ASSERT_ACTIVE_SCELL(rnti, uci_cfg.cqi.scell_index, /* void */);

  // Get CQI carrier index
  auto&    cqi_scell_info = ue_db.at(rnti).scell_info[uci_cfg.cqi.scell_index];
  uint32_t cqi_cc_idx     = cqi_scell_info.cc_idx;

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

    // Rank indicator (TM3 and TM4)
    if (uci_cfg.cqi.ri_len) {
      stack->ri_info(tti, cqi_cc_idx, rnti, uci_value.ri);
      cqi_scell_info.last_ri = uci_value.ri;
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
}

void phy_ue_db::set_last_ul_tb(uint16_t rnti, uint32_t cc_idx, uint32_t pid, srslte_ra_tb_t tb)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Assert UE DB entry
  UE_DB_ASSERT_ACTIVE_CELL(rnti, cc_idx, /* void */);

  // Save resource allocation
  ue_db.at(rnti).scell_info[_get_scell_idx(rnti, cc_idx)].last_tb[pid % SRSLTE_FDD_NOF_HARQ] = tb;
}

srslte_ra_tb_t phy_ue_db::get_last_ul_tb(uint16_t rnti, uint32_t cc_idx, uint32_t pid) const
{
  std::lock_guard<std::mutex> lock(mutex);

  // Assert UE DB entry
  UE_DB_ASSERT_ACTIVE_CELL(rnti, cc_idx, {});

  // Returns the latest stored UL transmission grant
  return ue_db.at(rnti).scell_info[_get_scell_idx(rnti, cc_idx)].last_tb[pid % SRSLTE_FDD_NOF_HARQ];
}
