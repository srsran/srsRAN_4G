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

#ifndef SRSRAN_UCI_CFG_NR_H
#define SRSRAN_UCI_CFG_NR_H

#include "csi_cfg.h"
#include "harq_ack_cfg.h"
#include "srsran/phy/common/phy_common_nr.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Maximum number of Uplink Control Bits
 * @remark TS 38.212 section 5.2.1 Polar coding: The value of A is no larger than 1706.
 */
#define SRSRAN_UCI_NR_MAX_NOF_BITS 1706U

/**
 * @brief Maximum number of Channel State Information part 1 (CSI1) bits that can be carried in Uplink Control
 * Information (UCI) message
 */
#define SRSRAN_UCI_NR_MAX_CSI1_BITS 360

/**
 * @brief Uplink Control Information bits configuration for PUCCH transmission
 */
typedef struct {
  uint16_t rnti;           ///< RNTI
  uint32_t resource_id;    ///< PUCCH resource indicator field in the DCI format 1_0 or DCI format 1_1
  uint32_t n_cce_0;        ///< index of a first CCE for the PDCCH reception
  uint32_t N_cce;          ///< number of CCEs in a CORESET of a PDCCH reception with DCI format 1_0 or 1_1
  uint32_t sr_resource_id; ///< Scheduling request resource identifier, only valid if positive SR
} srsran_uci_nr_pucch_cfg_t;

/**
 * @brief Uplink Control Information bits configuration for PUSCH transmission
 * @attention Set nof_layers, nof_re or R to 0 to indicate this structure is not initialised.
 */
typedef struct {
  uint32_t     l0; ///< First OFDM symbol that does not carry DMRS of the PUSCH, after the first DMRS symbol(s)
  uint32_t     l1; ///< OFDM symbol index of the first OFDM symbol that does not carry DMRS
  uint32_t     M_pusch_sc[SRSRAN_NSYMB_PER_SLOT_NR]; ///< Number of potential RE for PUSCH transmission
  uint32_t     M_uci_sc[SRSRAN_NSYMB_PER_SLOT_NR];   ///< Number of potential RE for UCI transmission
  uint32_t     K_sum;                                ///< Sum of UL-SCH code block sizes, set to zero if no UL-SCH
  srsran_mod_t modulation;                           ///< Modulation for the PUSCH
  uint32_t     nof_layers;                           ///< Number of layers for PUSCH
  uint32_t     nof_re;                               ///< Total number of resource elements allocated for the grant
  float        R;                                    ///< Code rate of the PUSCH
  float        alpha;                                ///< Higher layer parameter scaling
  float        beta_harq_ack_offset;
  float        beta_csi1_offset;
  float        beta_csi2_offset;
  bool         csi_part2_present;
} srsran_uci_nr_pusch_cfg_t;

/**
 * @brief Uplink Control Information (UCI) message configuration
 */
typedef struct SRSRAN_API {
  /// Common Parameters
  srsran_harq_ack_cfg_t   ack;                                 ///< HARQ-ACK configuration
  uint32_t                o_sr;                                ///< Number of SR bits
  bool                    sr_positive_present;                 ///< Set to true if there is at least one positive SR
  srsran_csi_report_cfg_t csi[SRSRAN_CSI_SLOT_MAX_NOF_REPORT]; ///< CSI report configuration
  uint32_t                nof_csi;                             ///< Number of CSI reports
  union {
    srsran_uci_nr_pucch_cfg_t pucch; ///< Configuration for transmission in PUCCH
    srsran_uci_nr_pusch_cfg_t pusch; ///< Configuration for transmission in PUSCH
  };
} srsran_uci_cfg_nr_t;

/**
 * @brief Uplink Control Information (UCI) message packed information
 */
typedef struct SRSRAN_API {
  uint8_t                   ack[SRSRAN_HARQ_ACK_MAX_NOF_BITS];   ///< HARQ ACK feedback bits
  uint32_t                  sr;                                  ///< Number of positive SR
  srsran_csi_report_value_t csi[SRSRAN_CSI_SLOT_MAX_NOF_REPORT]; ///< Packed CSI report values
  bool valid; ///< Indicates whether the message has been decoded successfully, ignored in the transmitter
} srsran_uci_value_nr_t;

/**
 * @brief Uplink Control Information (UCI) data (configuration + values)
 */
typedef struct SRSRAN_API {
  srsran_uci_cfg_nr_t   cfg;
  srsran_uci_value_nr_t value;
} srsran_uci_data_nr_t;

#endif // SRSRAN_UCI_CFG_NR_H
