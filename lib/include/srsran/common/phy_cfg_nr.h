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

#ifndef SRSRAN_PHY_CFG_NR_H
#define SRSRAN_PHY_CFG_NR_H

#include "srsran/config.h"
#include "srsran/srsran.h"
#include <array>
#include <srsran/adt/bounded_vector.h>
#include <string>

namespace srsran {

/***************************
 *      PHY Config
 **************************/

struct phy_cfg_nr_t {
  /**
   * SSB configuration
   */
  struct ssb_cfg_t {
    uint32_t                                    periodicity_ms    = 0;
    std::array<bool, SRSRAN_SSB_NOF_CANDIDATES> position_in_burst = {};
    srsran_subcarrier_spacing_t                 scs               = srsran_subcarrier_spacing_30kHz;
    srsran_ssb_pattern_t                        pattern           = SRSRAN_SSB_PATTERN_A;
  };

  srsran_duplex_config_nr_t duplex   = {};
  srsran_sch_hl_cfg_nr_t    pdsch    = {};
  srsran_sch_hl_cfg_nr_t    pusch    = {};
  srsran_pucch_nr_hl_cfg_t  pucch    = {};
  srsran_prach_cfg_t        prach    = {};
  srsran_pdcch_cfg_nr_t     pdcch    = {};
  srsran_harq_ack_cfg_hl_t  harq_ack = {};
  srsran_csi_hl_cfg_t       csi      = {};
  srsran_carrier_nr_t       carrier  = {};
  ssb_cfg_t                 ssb      = {};
  uint32_t                  t_offset = 0; ///< n-TimingAdvanceOffset

  phy_cfg_nr_t() {}

  bool carrier_is_equal(const srsran::phy_cfg_nr_t& other) const
  {
    return srsran_carrier_nr_equal(&carrier, &other.carrier);
  }

  /**
   * @brief Computes the DCI configuration for the current UE configuration
   */
  srsran_dci_cfg_nr_t get_dci_cfg() const;

  /**
   * @brief Asserts that the PDCCH configuration is valid for a given Search Space identifier
   * @param ss_id Identifier
   * @return true if the configuration is valid, false otherwise
   */
  bool assert_ss_id(uint32_t ss_id) const;

  /**
   * @brief Calculates the DCI location candidates for a given search space and aggregation level
   * @param slot_idx Slot index
   * @param rnti UE temporal identifier
   * @param ss_id Search Space identifier
   * @param L Aggregation level
   * @param[out] locations DCI candidate locations
   * @return true if the configuration is valid, false otherwise
   */
  bool get_dci_locations(
      const uint32_t&                                                                           slot_idx,
      const uint16_t&                                                                           rnti,
      const uint32_t&                                                                           ss_id,
      const uint32_t&                                                                           L,
      srsran::bounded_vector<srsran_dci_location_t, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR>& locations) const;

  /**
   * @brief Selects a valid DCI format for scheduling PDSCH and given Search Space identifier
   * @param ss_id Identifier
   * @return A valid DCI format if available, SRSRAN_DCI_FORMAT_NR_COUNT otherwise
   */
  srsran_dci_format_nr_t get_dci_format_pdsch(uint32_t ss_id) const;

  /**
   * @brief Selects a valid DCI format for scheduling PUSCH and given Search Space identifier
   * @param ss_id Identifier
   * @return A valid DCI format if available, SRSRAN_DCI_FORMAT_NR_COUNT otherwise
   */
  srsran_dci_format_nr_t get_dci_format_pusch(uint32_t ss_id) const;

  /**
   * @brief Fills PDSCH DCI context for C-RNTI using a search space identifier, DCI candidate location and RNTI
   */
  bool get_dci_ctx_pdsch_rnti_c(uint32_t                     ss_id,
                                const srsran_dci_location_t& location,
                                const uint16_t&              rnti,
                                srsran_dci_ctx_t&            ctx) const;

  /**
   * @brief Fills PUSCH DCI context for C-RNTI using a search space identifier, DCI candidate location and RNTI
   */
  bool get_dci_ctx_pusch_rnti_c(uint32_t                     ss_id,
                                const srsran_dci_location_t& location,
                                const uint16_t&              rnti,
                                srsran_dci_ctx_t&            ctx) const;

  /**
   * @brief Get PDSCH configuration for a given slot and DCI
   */
  bool
  get_pdsch_cfg(const srsran_slot_cfg_t& slot_cfg, const srsran_dci_dl_nr_t& dci, srsran_sch_cfg_nr_t& pdsch_cfg) const;

  /**
   * @brief Get PUSCH configuration for a given slot and DCI
   */
  bool
  get_pusch_cfg(const srsran_slot_cfg_t& slot_cfg, const srsran_dci_ul_nr_t& dci, srsran_sch_cfg_nr_t& pdsch_cfg) const;

  /**
   * @brief Get PDSCH ACK resource for a given PDSCH transmission
   */
  bool get_pdsch_ack_resource(const srsran_dci_dl_nr_t& dci_dl, srsran_harq_ack_resource_t& ack_resource) const;

  /**
   * @brief Compute UCI configuration for the given slot from the pending PDSCH ACK resources, periodic CSI,
   * periodic SR resources and so on.
   */
  bool get_uci_cfg(const srsran_slot_cfg_t&     slot_cfg,
                   const srsran_pdsch_ack_nr_t& pdsch_ack,
                   srsran_uci_cfg_nr_t&         uci_cfg) const;

  /**
   * @brief Compute UCI configuration for PUCCH for the given slot from the pending PDSCH ACK resources, periodic CSI,
   * periodic SR resources and so on.
   */
  bool get_pucch_uci_cfg(const srsran_slot_cfg_t&      slot_cfg,
                         const srsran_uci_cfg_nr_t&    uci_cfg,
                         srsran_pucch_nr_common_cfg_t& cfg,
                         srsran_pucch_nr_resource_t&   resource) const;

  /**
   * @brief Compute UCI configuration for PUSCH for the given slot from the pending PDSCH ACK resources, periodic CSI,
   * periodic SR resources and so on.
   */
  bool get_pusch_uci_cfg(const srsran_slot_cfg_t&   slot_cfg,
                         const srsran_uci_cfg_nr_t& uci_cfg,
                         srsran_sch_cfg_nr_t&       pusch_cfg) const;

  /**
   * @brief Generate SSB configuration from the overall configuration
   * @attention Sampling rate is the only parameter missing
   * @return valid SSB configuration
   */
  srsran_ssb_cfg_t get_ssb_cfg() const;
};

} // namespace srsran

#endif // SRSRAN_PHY_CFG_NR_H
