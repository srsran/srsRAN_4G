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

/******************************************************************************
 *  File:         ra_nr.h
 *
 *  Description:  Implements Resource allocation Procedures common in for DL and UL
 *
 *  Reference:    3GPP TS 38.214 version 15.10.0
 *****************************************************************************/

#ifndef SRSRAN_RA_NR_H
#define SRSRAN_RA_NR_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/phch/dci_nr.h"
#include "srsran/phy/phch/phch_cfg_nr.h"

/**
 * @brief Determines target rate
 * @param mcs_table Configured MCS table
 * @param dci_format DCI format used for the grant
 * @param search_space_type Search space type
 * @param rnti_type RNTI type
 * @param mcs_idx Desired Modulation Coding Scheme (MCS) index
 * @return The target rate if provided information is valid. Otherwise, it returns NAN
 */
SRSRAN_API double srsran_ra_nr_R_from_mcs(srsran_mcs_table_t         mcs_table,
                                          srsran_dci_format_nr_t     dci_format,
                                          srsran_search_space_type_t search_space_type,
                                          srsran_rnti_type_t         rnti_type,
                                          uint32_t                   mcs_idx);

/**
 * @brief Determines target rate
 * @param mcs_table Configured MCS table
 * @param dci_format DCI format used for the grant
 * @param search_space_type Search space type
 * @param rnti_type RNTI type
 * @param mcs_idx Desired Modulation Coding Scheme (MCS) index
 * @return The selected modulation if provided information is valid. Otherwise, it returns SRSRAN_MOD_NITEMS
 */
SRSRAN_API srsran_mod_t srsran_ra_nr_mod_from_mcs(srsran_mcs_table_t         mcs_table,
                                                  srsran_dci_format_nr_t     dci_format,
                                                  srsran_search_space_type_t search_space_type,
                                                  srsran_rnti_type_t         rnti_type,
                                                  uint32_t                   mcs_idx);

/**
 * @brief Determines the number of resource elements available for a given PDSCH transmission
 * @param pdsch_cfg PDSCH configuration provided by higher layers
 * @param grant The given PDSCH transmission grant
 * @return The number of resource elements if the provided configuration is valid, otherwise SRSRAN_ERROR code
 */
SRSRAN_API int srsran_ra_dl_nr_slot_nof_re(const srsran_sch_cfg_nr_t* pdsch_cfg, const srsran_sch_grant_nr_t* grant);

/**
 * @brief Calculates shared channel TBS
 * @param N_re Number of resource elements
 * @param S TB scaling, used in rare occasions. Set to 1, 0, NAN or INF if unused.
 * @param R Target Rate
 * @param Qm Modulation level
 * @param nof_layers Number of layers
 * @return The transport block size
 */
SRSRAN_API uint32_t srsran_ra_nr_tbs(uint32_t N_re, double S, double R, uint32_t Qm, uint32_t nof_layers);

SRSRAN_API int srsran_ra_nr_fill_tb(const srsran_sch_cfg_nr_t*   pdsch_cfg,
                                    const srsran_sch_grant_nr_t* grant,
                                    uint32_t                     mcs_idx,
                                    srsran_sch_tb_t*             tb);
/**
 * @brief Converts an unpacked DL DCI message to a PDSCH grant structure.
 * Implements the procedures defined in Section 5 of 38.214 to compute the resource allocation (5.1.2)
 * and modulation order, target rate, redundancy version and TBS (5.1.3)
 *
 * Note: Only TypeA PDSCH mapping type is supported
 *
 * @param carrier Carrier information struct
 * @param slot Slot configuration
 * @param pdsch_cfg PDSCH configuration indicated by higher layers
 * @param dci_dl DCI downlink (format 1_0 or 1_1)
 * @param pdsch_grant Generated PDSCH grant
 * @return 0 on success, -1 on error
 */
SRSRAN_API int srsran_ra_dl_dci_to_grant_nr(const srsran_carrier_nr_t*    carrier,
                                            const srsran_slot_cfg_t*      slot,
                                            const srsran_sch_hl_cfg_nr_t* pdsch_cfg,
                                            const srsran_dci_dl_nr_t*     dci_dl,
                                            srsran_sch_cfg_nr_t*          cfg,
                                            srsran_sch_grant_nr_t*        pdsch_grant);

/**
 * @brief Converts an unpacked UL DCI message to a PUSCH grant structure.
 * Implements the procedures defined in Section 6 of 38.214 to compute the resource allocation (6.1.2)
 * and modulation order, target rate, redundancy version and TBS (6.1.4)
 *
 * Note: Only TypeA PUSCH mapping type is supported
 *
 * @param carrier Carrier information struct
 * @param slot Slot configuration
 * @param pusch_hl_cfg PUSCH configuration provided by higher layers
 * @param dci_ul DCI uplink (format 0_0 or 0_1)
 * @param pusch_cfg PUSCH configuration after applying the procedure
 * @param pusch_grant Generated PUSCH grant
 * @return 0 on success, -1 on error
 */
SRSRAN_API int srsran_ra_ul_dci_to_grant_nr(const srsran_carrier_nr_t*    carrier,
                                            const srsran_slot_cfg_t*      slot,
                                            const srsran_sch_hl_cfg_nr_t* pusch_hl_cfg,
                                            const srsran_dci_ul_nr_t*     dci_ul,
                                            srsran_sch_cfg_nr_t*          pusch_cfg,
                                            srsran_sch_grant_nr_t*        pusch_grant);

/**
 * @brief Set up the Uplink Control Information configuration for a PUSCH transmission
 *
 * @remark Implement procedure described in TS 38.213 9.3 UCI reporting in physical uplink shared channel
 *
 * @param carrier Carrier information struct
 * @param pusch_hl_cfg PUSCH configuration provided by higher layers
 * @param uci_cfg Uplink Control Information configuration for this PUSCH transmission
 * @param pusch_cfg PUSCH configuration after applying the procedure
 * @return SRSRAN_SUCCESS if the procedure is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ra_ul_set_grant_uci_nr(const srsran_carrier_nr_t*    carrier,
                                             const srsran_sch_hl_cfg_nr_t* pusch_hl_cfg,
                                             const srsran_uci_cfg_nr_t*    uci_cfg,
                                             srsran_sch_cfg_nr_t*          pusch_cfg);

/**
 * @brief Calculates frequency allocation type 1 RIV field
 * @param N_rb Number of resource blocks
 * @param start_rb Start resource block index
 * @param length_rb Number of resource blocks
 * @return The RIV field with the encoded value
 */
SRSRAN_API uint32_t srsran_ra_nr_type1_riv(uint32_t N_rb, uint32_t start_rb, uint32_t length_rb);

/**
 * @brief Returns the MCS corresponding to CQI
 *
 * Mapping is performed as: return the MCS that has the closest spectral efficiency to that of the CQI
 *
 * @remark Implements mapping based on TS 38.214, MCS Tables 5.1.3.1-1, 5.1.3.1-2, 5.1.3.1-3 and CQI Tables 5.2.2.1-2,
 * Table 5.2.2.1-3, 5.2.2.1-4
 * @param cqi CQI value
 * @param cqi_table_idx CQI table index
 * @param mcs_table MCS table parameter
 * @param dci_format DCI format
 * @param search_space_type Seach Space type
 * @param rnti_type RNTI type
 * @return The MCS index
 */
SRSRAN_API int srsran_ra_nr_cqi_to_mcs(uint8_t                    cqi,
                                       srsran_csi_cqi_table_t     cqi_table_idx,
                                       srsran_mcs_table_t         mcs_table,
                                       srsran_dci_format_nr_t     dci_format,
                                       srsran_search_space_type_t search_space_type,
                                       srsran_rnti_type_t         rnti_type);

/**
 * @brief Returns the Spectral Efficiency corresponding to CQI
 *
 * Mapping is performed as: return the MCS that has the closest spectral efficiency to that of the CQI
 *
 * @remark Implements mapping based on TS 38.214, CQI Tables 5.2.2.1-2, Table 5.2.2.1-3, 5.2.2.1-4
 * @param cqi CQI value
 * @param cqi_table_idx CQI table index
 * @return The Spectral Efficiency
 */
SRSRAN_API double srsran_ra_nr_cqi_to_se(uint8_t cqi, srsran_csi_cqi_table_t cqi_table_idx);

/**
 * @brief Returns the MCS corresponding to Spectral Efficiency
 *
 * Mapping is performed as: return the greatest MCS with an SE lower than or equal to target SE
 *
 * @remark Implements mapping based on TS 38.214, MCS Tables 5.1.3.1-1, 5.1.3.1-2, 5.1.3.1-3
 * @param se_target Target Spectral efficiency to be mapped into MCS
 * @param mcs_table MCS table parameter
 * @param dci_format DCI format
 * @param search_space_type Seach Space type
 * @param rnti_type RNTI type
 * @return The MCS index
 */
SRSRAN_API int srsran_ra_nr_se_to_mcs(double                     se_target,
                                      srsran_mcs_table_t         mcs_table,
                                      srsran_dci_format_nr_t     dci_format,
                                      srsran_search_space_type_t search_space_type,
                                      srsran_rnti_type_t         rnti_type);

#endif // SRSRAN_RA_NR_H
