/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSLTE_UCI_CFG_NR_H
#define SRSLTE_UCI_CFG_NR_H

#include "srslte/phy/common/phy_common.h"
#include <stdbool.h>
#include <stdint.h>

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
  uint32_t     o_ack;      ///< Number of HARQ-ACK bits
  uint32_t     o_sr;       ///< Number of SR bits
  uint32_t     o_csi1;     ///< Number of CSI1 report number of bits
  uint32_t     o_csi2;     ///< Number of CSI2 report number of bits
  srslte_mod_t modulation; ///< Modulation (PUSCH only)
  uint16_t     rnti;       ///< RNTI
} srslte_uci_cfg_nr_t;

/**
 * @brief Uplink Control Information (UCI) message packed information
 */
typedef struct SRSLTE_API {
  uint8_t ack[SRSLTE_UCI_NR_MAX_ACK_BITS];   ///< HARQ ACK feedback bits
  uint8_t sr[SRSLTE_UCI_NR_MAX_SR_BITS];     ///< Scheduling Request bits
  uint8_t csi1[SRSLTE_UCI_NR_MAX_CSI1_BITS]; ///< Channel State Information part 1
  uint8_t csi2[SRSLTE_UCI_NR_MAX_CSI2_BITS]; ///< Channel State Information part 2
  bool    valid; ///< Indicates whether the message has been decoded successfully, ignored in the transmitter
} srslte_uci_value_nr_t;

#endif // SRSLTE_UCI_CFG_NR_H
