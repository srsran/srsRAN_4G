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

#ifndef SRSRAN_PUCCH_PROC_H_
#define SRSRAN_PUCCH_PROC_H_

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_ul.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/phch/pucch.h"
#include "srsran/phy/phch/uci.h"

#define SRSRAN_PUCCH_MAX_ALLOC 4

/**
 * Implements 3GPP 36.213 R10 10.1 PUCCH format selection
 * @param cell cell configuration
 * @param cfg PUCCH configuration struct
 * @param uci_cfg uplink control information configuration
 * @param uci_value uplink control information data, use NULL if unavailable
 * @return
 */
SRSRAN_API srsran_pucch_format_t srsran_pucch_proc_select_format(const srsran_cell_t*      cell,
                                                                 const srsran_pucch_cfg_t* cfg,
                                                                 const srsran_uci_cfg_t*   uci_cfg,
                                                                 const srsran_uci_value_t* uci_value);

/**
 * Determines the possible n_pucch_i resources/locations for PUCCH. It follows 3GPP 36.213 R10 10.1 UE procedure for
 * determining physical uplink control channel assignment.
 *
 * @param cell cell configuration
 * @param cfg PUCCH configuration struct
 * @param uci_cfg uplink control information configuration
 * @param uci_value uplink control information value
 * @param n_pucch_i table with the PUCCH format 1b possible resources
 * @return Returns the number of entries in the table or negative value indicating error
 */
SRSRAN_API int srsran_pucch_proc_get_resources(const srsran_cell_t*      cell,
                                               const srsran_pucch_cfg_t* cfg,
                                               const srsran_uci_cfg_t*   uci_cfg,
                                               const srsran_uci_value_t* uci_value,
                                               uint32_t*                 n_pucch_i);
/**
 * Determines the n_pucch resource for PUCCH.
 * @param cell Cell setup
 * @param cfg PUCCH configuration
 * @param uci_cfg UCI configuration (CS and TDD HARQ-ACK modes may modify this value)
 * @param uci_value UCI Value (CS and TDD HARQ-ACK modes may modify this value)
 * @param b Selected ACK-NACK bits after HARQ-ACK feedback mode "encoding"
 * @return the selected resource
 */
SRSRAN_API uint32_t srsran_pucch_proc_get_npucch(const srsran_cell_t*      cell,
                                                 const srsran_pucch_cfg_t* cfg,
                                                 const srsran_uci_cfg_t*   uci_cfg,
                                                 const srsran_uci_value_t* uci_value,
                                                 uint8_t                   b[SRSRAN_UCI_MAX_ACK_BITS]);

/**
 * Decodes the HARQ ACK bits from a selected resource (j) and received bits (b)
 * 3GPP 36.213 R10 10.1.2.2.1 PUCCH format 1b with channel selection HARQ-ACK procedure
 * tables:
 *  - Table 10.1.2.2.1-3: Transmission of Format 1b HARQ-ACK channel selection for A = 2
 *  - Table 10.1.2.2.1-4: Transmission of Format 1b HARQ-ACK channel selection for A = 3
 *  - Table 10.1.2.2.1-5: Transmission of Format 1b HARQ-ACK channel selection for A = 4
 * @param cfg PUCCH configuration struct
 * @param uci_cfg uplink control information configuration
 * @param j selected channel
 * @param b received bits
 * @return Returns SRSRAN_SUCCESS if it can decode it successfully, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_pucch_cs_get_ack(const srsran_pucch_cfg_t* cfg,
                                       const srsran_uci_cfg_t*   uci_cfg,
                                       uint32_t                  j,
                                       const uint8_t             b[SRSRAN_PUCCH_1B_2B_NOF_ACK],
                                       srsran_uci_value_t*       uci_value);

#endif // SRSRAN_PUCCH_PROC_H_
