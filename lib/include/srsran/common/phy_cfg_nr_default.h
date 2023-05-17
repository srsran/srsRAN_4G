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

#ifndef SRSRAN_PHY_CFG_NR_DEFAULT_H
#define SRSRAN_PHY_CFG_NR_DEFAULT_H

#include "phy_cfg_nr.h"

namespace srsran {

class phy_cfg_nr_default_t : public phy_cfg_nr_t
{
public:
  struct reference_cfg_t {
    enum {
      /**
       * @brief Carrier reference configuration for 10MHz serving cell bandwidth
       * - BW: 10 MHZ (52 PRB)
       * - PCI: 500
       * - SCS: 15 kHz
       * - SSB: 5ms
       */
      R_CARRIER_CUSTOM_10MHZ = 0,
      /**
       * @brief Carrier reference configuration for 10MHz serving cell bandwidth
       * - BW: 20 MHZ (106 PRB)
       * - PCI: 500
       * - SCS: 15 kHz
       * - SSB: 5ms
       */
      R_CARRIER_CUSTOM_20MHZ,
      R_CARRIER_COUNT
    } carrier                                                       = R_CARRIER_CUSTOM_10MHZ;
    const std::array<std::string, R_CARRIER_COUNT> R_CARRIER_STRING = {{"10MHz", "20MHz"}};

    enum {
      /**
       * @brief FDD, all slots for DL and UL
       */
      R_DUPLEX_FDD = 0,

      /**
       * @brief TDD custom reference 5 slot DL and 5 slot UL
       */
      R_DUPLEX_TDD_CUSTOM_6_4,

      /**
       * @brief TDD pattern FR1.15-1 defined in TS38.101-4 Table A.1.2-1
       */
      R_DUPLEX_TDD_FR1_15_1,
      R_DUPLEX_COUNT,
    } duplex                                                      = R_DUPLEX_TDD_CUSTOM_6_4;
    const std::array<std::string, R_DUPLEX_COUNT> R_DUPLEX_STRING = {{"FDD", "6D+4U", "FR1.15-1"}};

    enum {
      /**
       * @brief Carrier reference configuration for 10MHz serving cell bandwidth
       * - CORESET: all channel, 1 symbol
       * - Single common Search Space
       * - 2 possible candidate per aggregation level to allow DL and UL grants simultaneously
       */
      R_PDCCH_CUSTOM_COMMON_SS = 0,
    } pdcch = R_PDCCH_CUSTOM_COMMON_SS;

    enum {
      /**
       * @brief Custom fallback baseline configuration, designed for component testing
       * - Defined single common PDSCH time allocation starting at symbol index 1 and length 13
       * - No DMRS dedicated configuration
       */
      R_PDSCH_DEFAULT = 0,

      /**
       * @brief PDSCH parameters described in TS 38.101-4 Table 5.2.2.2.1-2 for the test described in table 5.2.2.2.1-3
       */
      R_PDSCH_TS38101_5_2_1,

      /**
       * @brief Invalid PDSCH reference channel
       */
      R_PDSCH_COUNT

    } pdsch                                                     = R_PDSCH_DEFAULT;
    const std::array<std::string, R_PDSCH_COUNT> R_PDSCH_STRING = {{"default", "ts38101/5.2-1"}};

    enum {
      /**
       * @brief Custom fallback baseline configuration, designed for component testing
       * - Single Time resource allocation
       * - transmission starts at symbol index 0 for 14 symbols
       * - k is 4 slots
       * - No DMRS dedicated configuration
       */
      R_PUSCH_DEFAULT = 0,
    } pusch = R_PUSCH_DEFAULT;

    enum {
      /**
       * @brief Custom single PUCCH resource per set
       * - Format 1 for 1 or 2 bits
       * - Format 2 for more than 2 bits
       */
      R_PUCCH_CUSTOM_ONE = 0,
    } pucch = R_PUCCH_CUSTOM_ONE;

    enum {
      /**
       * @brief Sets the delay between PDSCH and HARQ feedback timing automatically
       * - Dynamic HARQ ACK codebook
       * - Guarantees a minimum delay of 4ms
       * - Assume 15kHz SCS
       * - Assume TDD pattern2 is not enabled
       */
      R_HARQ_AUTO = 0,
    } harq = R_HARQ_AUTO;

    enum {
      /**
       * @brief Sets the PRACH configuration to an LTE compatible configuration
       * - Configuration index 0
       * - Frequency offset 2 PRB
       * - Root sequence 2
       */
      R_PRACH_DEFAULT_LTE,
    } prach = R_PRACH_DEFAULT_LTE;

    reference_cfg_t() = default;
    explicit reference_cfg_t(const std::string& args);
  };

  phy_cfg_nr_default_t(const reference_cfg_t& reference_cfg);

private:
  /**
   * Carrier make helper methods
   */
  static void make_carrier_custom_10MHz(srsran_carrier_nr_t& carrier);
  static void make_carrier_custom_20MHz(srsran_carrier_nr_t& carrier);

  /**
   * TDD make helper methods
   */
  static void make_tdd_custom_6_4(srsran_duplex_config_nr_t& duplex);
  static void make_tdd_fr1_15_1(srsran_duplex_config_nr_t& duplex);

  /**
   * PDCCH make helper methods
   */
  static void make_pdcch_custom_common_ss(srsran_pdcch_cfg_nr_t& pdcch, const srsran_carrier_nr_t& carrier);

  /**
   * PDSCH make helper methods
   */
  static void make_pdsch_default(srsran_sch_hl_cfg_nr_t& pdsch);
  static void make_pdsch_2_1_1_tdd(const srsran_carrier_nr_t& carrier, srsran_sch_hl_cfg_nr_t& pdsch);

  /**
   * PUSCH make helper methods
   */
  static void make_pusch_default(srsran_sch_hl_cfg_nr_t& pusch);

  /**
   * PUCCH make helper methods
   */
  static void make_pucch_custom_one(srsran_pucch_nr_hl_cfg_t& pucch);

  /**
   * HARQ make helper methods
   */
  static void make_harq_auto(srsran_harq_ack_cfg_hl_t&        harq,
                             const srsran_carrier_nr_t&       carrier,
                             const srsran_duplex_config_nr_t& duplex_cfg);

  /**
   * PRACH make helper methods
   */
  static void make_prach_default_lte(srsran_prach_cfg_t& prach);
};

} // namespace srsran

#endif // SRSRAN_PHY_CFG_NR_DEFAULT_H
