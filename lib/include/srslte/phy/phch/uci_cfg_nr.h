/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_UCI_CFG_NR_H
#define SRSLTE_UCI_CFG_NR_H

#include "csi_cfg.h"
#include "srslte/phy/common/phy_common.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Maximum number of Uplink Control Bits
 * @remark TS 38.212 section 5.2.1 Polar coding: The value of A is no larger than 1706.
 */
#define SRSLTE_UCI_NR_MAX_NOF_BITS 1706U

/**
 * @brief Maximum number of HARQ ACK feedback bits that can be carried in Uplink Control Information (UCI) message
 */
#define SRSLTE_UCI_NR_MAX_ACK_BITS 360

/**
 * @brief Maximum number of Scheduling Request (SR) bits that can be carried in Uplink Control Information (UCI) message
 */
#define SRSLTE_UCI_NR_MAX_SR_BITS 10

/**
 * @brief Maximum number of Channel State Information part 1 (CSI1) bits that can be carried in Uplink Control
 * Information (UCI) message
 */
#define SRSLTE_UCI_NR_MAX_CSI1_BITS 10

/**
 * @brief Maximum number of Channel State Information part 2 (CSI2) bits that can be carried in Uplink Control
 * Information (UCI) message
 */
#define SRSLTE_UCI_NR_MAX_CSI2_BITS 10

/**
 * @brief Uplink Control Information (UCI) message configuration
 */
typedef struct SRSLTE_API {
  /// Common Parameters
  uint32_t                o_ack;                          ///< Number of HARQ-ACK bits
  uint32_t                o_sr;                           ///< Number of SR bits
  srslte_csi_report_cfg_t csi[SRSLTE_CSI_MAX_NOF_REPORT]; ///< CSI report configuration
  uint32_t                nof_csi;                        ///< Number of CSI reports

  /// PUSCH only parameters
  srslte_mod_t modulation; ///< Modulation

  /// PUCCH only parameters
  uint16_t rnti;                ///< RNTI
  uint32_t pucch_resource_id;   ///< PUCCH resource indicator field in the DCI format 1_0 or DCI format 1_1
  uint32_t n_cce_0;             ///< index of a first CCE for the PDCCH reception
  uint32_t N_cce;               ///< number of CCEs in a CORESET of a PDCCH reception with DCI format 1_0 or 1_1
  uint32_t sr_resource_id;      ///< Scheduling request resource identifier, only valid if positive SR
  bool     sr_positive_present; ///< Set to true if there is at least one positive SR
} srslte_uci_cfg_nr_t;

/**
 * @brief Uplink Control Information (UCI) message packed information
 */
typedef struct SRSLTE_API {
  uint8_t                   ack[SRSLTE_UCI_NR_MAX_ACK_BITS]; ///< HARQ ACK feedback bits
  uint32_t                  sr;                              ///< Number of positive SR
  srslte_csi_report_value_t csi[SRSLTE_CSI_MAX_NOF_REPORT];  ///< Packed CSI report values
  bool valid; ///< Indicates whether the message has been decoded successfully, ignored in the transmitter
} srslte_uci_value_nr_t;

/**
 * @brief Uplink Control Information (UCI) data (configuration + values)
 */
typedef struct SRSLTE_API {
  srslte_uci_cfg_nr_t   cfg;
  srslte_uci_value_nr_t value;
} srslte_uci_data_nr_t;

#endif // SRSLTE_UCI_CFG_NR_H
