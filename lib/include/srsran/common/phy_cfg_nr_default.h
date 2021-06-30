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
    } carrier = R_CARRIER_CUSTOM_10MHZ;

    enum {
      /**
       * @brief TDD custom reference 5 slot DL and 5 slot UL
       */
      R_TDD_CUSTOM_6_4 = 0,
    } tdd = R_TDD_CUSTOM_6_4;

    enum {
      /**
       * @brief Carrier reference configuration for 10MHz serving cell bandwidth
       * - CORESET: all channel, 1 symbol
       * - Single common Search Space
       * - 1 possible candidate per aggregation level
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
    } pdsch = R_PDSCH_DEFAULT;

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
  };

  phy_cfg_nr_default_t(const reference_cfg_t& reference_cfg);

private:
  /**
   * Carrier make helper methods
   */
  static void make_carrier_custom_10MHz(srsran_carrier_nr_t& carrier);

  /**
   * TDD make helper methods
   */
  static void make_tdd_custom_6_4(srsran_tdd_config_nr_t& tdd);

  /**
   * PDCCH make helper methods
   */
  static void make_pdcch_custom_common_ss(srsran_pdcch_cfg_nr_t& pdcch, const srsran_carrier_nr_t& carrier);

  /**
   * PDSCH make helper methods
   */
  static void make_pdsch_default(srsran_sch_hl_cfg_nr_t& pdsch);

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
  static void make_harq_auto(srsran_harq_ack_cfg_hl_t&     harq,
                             const srsran_carrier_nr_t&    carrier,
                             const srsran_tdd_config_nr_t& tdd_cfg);

  /**
   * PRACH make helper methods
   */
  static void make_prach_default_lte(srsran_prach_cfg_t& prach);
};

} // namespace srsran

#endif // SRSRAN_PHY_CFG_NR_DEFAULT_H
