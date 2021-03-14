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

#ifndef SRSLTE_CSI_NR_H
#define SRSLTE_CSI_NR_H

#include "uci_cfg_nr.h"

/**
 * @brief Fills Uplink Control Information data with triggered reports for the given slot
 * @param cfg CSI report configuration
 * @param slot_idx Slot index within the radio frame
 * @param measurements CSI measurements
 * @param[out] uci_data Uplink Control Information data
 * @return The number CSI reports for transmission if the provided data is valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_csi_generate_reports(const srslte_csi_hl_cfg_t*      cfg,
                                           uint32_t                        slot_idx,
                                           const srslte_csi_measurements_t measurements[SRSLTE_CSI_MAX_NOF_RESOURCES],
                                           srslte_csi_report_cfg_t         report_cfg[SRSLTE_CSI_MAX_NOF_REPORT],
                                           srslte_csi_report_value_t       report_value[SRSLTE_CSI_MAX_NOF_REPORT]);

/**
 * @brief Compute number of CSI bits necessary to transmit all the CSI reports for a PUCCH transmission
 * @param report_list Provides the CSI report list
 * @param nof_reports Number of CSI reports in the list
 * @return The number of bits if the provided list is valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_csi_part1_nof_bits(const srslte_csi_report_cfg_t* report_list, uint32_t nof_reports);

/**
 * @brief Checks if the report list contains part 2 CSI report
 * @param report_list Report list
 * @param nof_reports Number of reports in the list
 * @return True if at least one report contains part 2, false otherwise
 */
SRSLTE_API bool srslte_csi_has_part2(const srslte_csi_report_cfg_t* report_list, uint32_t nof_reports);

/**
 * @brief Pack CSI part 1 bits for a PUCCH or PUSCH transmission
 * @param report_list Provides the CSI report list
 * @param nof_reports Number of CSI reports in the list
 * @param o_csi1 CSI bits
 * @param max_o_csi1 Maximum number of CSI bits
 * @return number of packed bits if provided data is valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_csi_part1_pack(const srslte_csi_report_cfg_t*   report_cfg,
                                     const srslte_csi_report_value_t* report_value,
                                     uint32_t                         nof_reports,
                                     uint8_t*                         o_csi1,
                                     uint32_t                         max_o_csi1);

/**
 *@brief Unpacks CSI part 1 bits for PUCCH or PUSCH transmission
 * @param report_list Provides the CSI report list
 * @param nof_reports Number of CSI reports in the list
 * @param o_csi1 CSI bits
 * @param max_o_csi1 Maximum number of CSI bits
 * @param report_value
 * @return SRSLTE_SUCCESS if provided data is valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_csi_part1_unpack(const srslte_csi_report_cfg_t* report_cfg,
                                       uint32_t                       nof_reports,
                                       uint8_t*                       o_csi1,
                                       uint32_t                       max_o_csi1,
                                       srslte_csi_report_value_t*     report_value);

/**
 * @brief Converts to string a given list of CSI reports
 * @param report_cfg Report configuration list
 * @param report_value Report value list
 * @param nof_reports Number of reports
 * @param str String pointer
 * @param str_len Maximum string length
 * @return Number of used characters
 */
SRSLTE_API uint32_t srslte_csi_str(const srslte_csi_report_cfg_t*   report_cfg,
                                   const srslte_csi_report_value_t* report_value,
                                   uint32_t                         nof_reports,
                                   char*                            str,
                                   uint32_t                         str_len);

#endif // SRSLTE_CSI_NR_H
