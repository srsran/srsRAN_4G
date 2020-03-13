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

#ifndef SRSLTE_PUCCH_PROC_H_
#define SRSLTE_PUCCH_PROC_H_

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/chest_ul.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/common/sequence.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/phch/pucch.h"
#include "srslte/phy/phch/uci.h"

#define SRSLTE_PUCCH_MAX_ALLOC 4

/**
 * Implements 3GPP 36.213 R10 10.1 PUCCH format selection
 * @param cell cell configuration
 * @param cfg PUCCH configuration struct
 * @param uci_cfg uplink control information configuration
 * @param uci_value uplink control information data, use NULL if unavailable
 * @return
 */
SRSLTE_API srslte_pucch_format_t srslte_pucch_proc_select_format(const srslte_cell_t*      cell,
                                                                 const srslte_pucch_cfg_t* cfg,
                                                                 const srslte_uci_cfg_t*   uci_cfg,
                                                                 const srslte_uci_value_t* uci_value);

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
SRSLTE_API int srslte_pucch_proc_get_resources(const srslte_cell_t*      cell,
                                               const srslte_pucch_cfg_t* cfg,
                                               const srslte_uci_cfg_t*   uci_cfg,
                                               const srslte_uci_value_t* uci_value,
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
SRSLTE_API uint32_t srslte_pucch_proc_get_npucch(const srslte_cell_t*      cell,
                                                 const srslte_pucch_cfg_t* cfg,
                                                 const srslte_uci_cfg_t*   uci_cfg,
                                                 const srslte_uci_value_t* uci_value,
                                                 uint8_t                   b[SRSLTE_UCI_MAX_ACK_BITS]);

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
 * @return Returns SRSLTE_SUCCESS if it can decode it succesfully, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_pucch_cs_get_ack(const srslte_pucch_cfg_t* cfg,
                                       const srslte_uci_cfg_t*   uci_cfg,
                                       uint32_t                  j,
                                       const uint8_t             b[SRSLTE_PUCCH_1B_2B_NOF_ACK],
                                       srslte_uci_value_t*       uci_value);

#endif // SRSLTE_PUCCH_PROC_H_
