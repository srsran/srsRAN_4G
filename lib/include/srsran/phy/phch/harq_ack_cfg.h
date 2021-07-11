/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_HARQ_ACK_CFG_H
#define SRSRAN_HARQ_ACK_CFG_H

#include "srsran/phy/common/phy_common_nr.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Maximum number of HARQ ACK feedback bits that can be carried in Uplink Control Information (UCI) message
 */
#define SRSRAN_HARQ_ACK_MAX_NOF_BITS 360

/**
 * @brief Describes a HARQ ACK resource configuration
 */
typedef struct {
  uint32_t scell_idx;  ///< Serving cell index
  uint32_t v_dai_dl;   ///< Downlink Assigment Index
  bool dci_format_1_1; ///< Set to true if the PDSCH transmission is triggered by a PDCCH DCI format 1_1 transmission
  uint32_t k1;         ///< HARQ feedback timing
  uint32_t pid;        ///< HARQ process identifier
  uint16_t rnti;
  uint32_t pucch_resource_id;
} srsran_harq_ack_resource_t;

/**
 * @brief Describes a single PDSCH transmission HARQ ACK feedback
 */
typedef struct {
  srsran_harq_ack_resource_t resource;
  uint8_t                    value[SRSRAN_MAX_CODEWORDS]; // 0/1 or 2 for DTX
  bool present; // set to true if there is a PDSCH on serving cell c associated with PDCCH in PDCCH monitoring occasion
  // m, or there is a PDCCH indicating SPS PDSCH release on serving cell c
  bool dl_bwp_changed; // set to true if PDCCH monitoring occasion m is before an active DL BWP change on serving cell c
  bool ul_bwp_changed; // set to true if an active UL BWP change on the PCell and an active DL BWP change is not
  // triggered by a DCI format 1_1 in PDCCH monitoring occasion m
  bool second_tb_present; // set to true if two TB were detected in the PDCCH monitoring occasion m
} srsran_harq_ack_m_t;

#define SRSRAN_UCI_NR_MAX_M 10

/**
 * @brief Describes a collection of PDSCH HARQ ACK feedback for a number of PDSCH transmissions within a component
 * carrier
 */
typedef struct {
  uint32_t            M;
  srsran_harq_ack_m_t m[SRSRAN_UCI_NR_MAX_M];
} srsran_harq_ack_cc_t;

/**
 * @brief Describes a collection of PDSCH HARQ ACK feedback for a number of component carriers
 */
typedef struct {
  srsran_harq_ack_cc_t cc[SRSRAN_MAX_CARRIERS];
  uint32_t             nof_cc;
  bool                 use_pusch; // Set to true, if UCI bits are carried by PUSCH
} srsran_pdsch_ack_nr_t;

/**
 * @brief Describes higher layer HARQ ACK feedback for PDSCH configuration
 */
typedef struct SRSRAN_API {
  bool harq_ack_spatial_bundling_pucch;         ///< Param harq-ACK-SpatialBundlingPUCCH, set to true if provided
  bool harq_ack_spatial_bundling_pusch;         ///< Param harq-ACK-SpatialBundlingPUSCH, set to true if provided
  srsran_harq_ack_codebook_t harq_ack_codebook; ///< pdsch-HARQ-ACK-Codebook configuration
  bool max_cw_sched_dci_is_2; ///< Param maxNrofCodeWordsScheduledByDCI, set to true if present and equal to 2

  uint32_t dl_data_to_ul_ack[SRSRAN_MAX_NOF_DL_DATA_TO_UL];
  uint32_t nof_dl_data_to_ul_ack;
} srsran_harq_ack_cfg_hl_t;

/**
 * @brief Describes HARQ ACK bit configuration for UCI encoding/decoding
 * @note if tb0 and tb1 are false, the HARQ ACK bit is not used
 * @note if tb0 and tb1 are true, the HARQ ACK feedback are bundled, the actual HARQ ACK feedback bit is the result of
 * the AND operation
 */
typedef struct SRSRAN_API {
  bool     tb0;    ///< Set to true if this ACK bit is  mapped into TB index 0
  bool     tb1;    ///< Set to true if this ACK bit is  mapped into TB index 1
  uint32_t cc_idx; ///< Component carrier index
  uint32_t m_idx;  ///< M resource index for packing/unpacking
  uint32_t pid;    ///< HARQ process identifier
} srsran_harq_ack_bit_t;

/**
 * @brief Describes HARQ ACK feedback configuration for UCI encoding/decoding
 */
typedef struct SRSRAN_API {
  srsran_harq_ack_bit_t bits[SRSRAN_HARQ_ACK_MAX_NOF_BITS]; ///< HARQ-ACK feedback bits
  uint32_t              count;                              ///< Number of ACK bits
} srsran_harq_ack_cfg_t;

#endif // SRSRAN_HARQ_ACK_CFG_H
