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

#ifndef SRSRAN_DCI_NR_H
#define SRSRAN_DCI_NR_H

#include "dci.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/phch/phch_cfg_nr.h"

typedef struct SRSRAN_API {
  srsran_dci_location_t      location;
  srsran_search_space_type_t search_space;
  uint32_t                   coreset_id;
  uint8_t                    payload[50];
  srsran_rnti_type_t         rnti_type;
  uint32_t                   nof_bits;
  srsran_dci_format_nr_t     format;
  uint16_t                   rnti;
} srsran_dci_msg_nr_t;

typedef struct SRSRAN_API {
  uint16_t                   rnti;
  srsran_rnti_type_t         rnti_type;
  srsran_dci_format_nr_t     format;
  srsran_dci_location_t      location;
  srsran_search_space_type_t search_space;
  uint32_t                   coreset_id;

  // Common fields
  uint32_t freq_domain_assigment; ///< Frequency domain resource assignment
  uint32_t time_domain_assigment; ///< Time domain resource assignment
  uint32_t vrb_to_prb_mapping;    ///< VRB-to-PRB mapping
  uint32_t mcs;                   ///< Modulation and coding scheme
  uint32_t rv;                    ///< Redundancy version
  uint32_t reserved;              ///< Reserved bits

  // C-RNTI/TC-RNTI specific fields
  uint32_t ndi;            ///< New data indicator
  uint32_t pid;            ///< HARQ process number
  uint32_t dai;            ///< Downlink assignment index
  uint32_t tpc;            ///< TPC command for scheduled PUCCH
  uint32_t pucch_resource; ///< PUCCH resource indicator
  uint32_t harq_feedback;  ///< PDSCH-to-HARQ_feedback timing indicator

  // P-RNTI specific fields
  uint32_t smi;        ///< Short Messages Indicator
  uint32_t sm;         ///< Short Messages
  uint32_t tb_scaling; ///< TB scaling

  // SI-RNTI specific fields
  uint32_t sii; ///< System information indicator

} srsran_dci_dl_nr_t;

typedef struct SRSRAN_API {
  // Context information
  uint16_t                   rnti;
  srsran_rnti_type_t         rnti_type;
  srsran_dci_format_nr_t     format;
  srsran_dci_location_t      location;
  srsran_search_space_type_t search_space;
  uint32_t                   coreset_id;

  // Common fields
  uint32_t freq_domain_assigment; ///< Frequency domain resource assignment
  uint32_t time_domain_assigment; ///< Time domain resource assignment
  uint32_t freq_hopping_flag;     ///< Frequency hopping flag
  uint32_t mcs;                   ///< Modulation and coding scheme
  uint32_t rv;                    ///< Redundancy version
  uint32_t reserved;              ///< Reserved bits

  // C-RNTI or CS-RNTI or MCS-C-RNTI
  uint32_t ndi;            ///< New data indicator
  uint32_t pid;            ///< HARQ process number
  uint32_t tpc;            ///< TPC command for scheduled PUCCH
  uint32_t pucch_resource; ///< PUCCH resource indicator
  uint32_t harq_feedback;  ///< PDSCH-to-HARQ_feedback timing indicator

  // Frequency hopping
  uint32_t frequency_offset; ///< frequency offset

  // Random Access Response Grant
  uint32_t csi_request;
} srsran_dci_ul_nr_t;

/**
 * @brief Indicates whether the provided DCI message format bit indicator belongs to DCI format 1_0 according according
 * to the RNTI type. If invalid, the DCI message is likely to be format 0_0
 * @param dci_msg Provides DCI format 1_0 message
 * @return true if the DCI message is format 1_0, false otherwise
 */
SRSRAN_API bool srsran_dci_nr_format_1_0_valid(const srsran_dci_msg_nr_t* dci_msg);

SRSRAN_API int srsran_dci_nr_pack(const srsran_carrier_nr_t* carrier,
                                  const srsran_coreset_t*    coreset,
                                  const srsran_dci_dl_nr_t*  dci,
                                  srsran_dci_msg_nr_t*       msg);

SRSRAN_API int srsran_dci_nr_format_0_0_sizeof(const srsran_carrier_nr_t* carrier,
                                               const srsran_coreset_t*    coreset,
                                               srsran_rnti_type_t         rnti_type);

SRSRAN_API int srsran_dci_nr_format_0_0_pack(const srsran_carrier_nr_t* carrier,
                                             const srsran_coreset_t*    coreset0,
                                             const srsran_dci_ul_nr_t*  dci,
                                             srsran_dci_msg_nr_t*       msg);

SRSRAN_API int srsran_dci_nr_format_0_0_unpack(const srsran_carrier_nr_t* carrier,
                                               const srsran_coreset_t*    coreset,
                                               srsran_dci_msg_nr_t*       msg,
                                               srsran_dci_ul_nr_t*        dci);

/**
 * @brief Unpacks DCI from Random Access Response Grant
 * @remark Described in TS 38.213 Table 8.2-1: Random Access Response Grant Content field size
 * @param msg
 * @param dci
 * @return SRSRAN_SUCCESS if unpacked correctly, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dci_nr_rar_unpack(srsran_dci_msg_nr_t* msg, srsran_dci_ul_nr_t* dci);

SRSRAN_API int srsran_dci_nr_format_1_0_sizeof(const srsran_carrier_nr_t* carrier,
                                               const srsran_coreset_t*    coreset,
                                               srsran_rnti_type_t         rnti_type);

SRSRAN_API int srsran_dci_nr_format_1_0_pack(const srsran_carrier_nr_t* carrier,
                                             const srsran_coreset_t*    coreset,
                                             const srsran_dci_dl_nr_t*  dci,
                                             srsran_dci_msg_nr_t*       msg);

SRSRAN_API int srsran_dci_nr_format_1_0_unpack(const srsran_carrier_nr_t* carrier,
                                               const srsran_coreset_t*    coreset,
                                               srsran_dci_msg_nr_t*       msg,
                                               srsran_dci_dl_nr_t*        dci);

SRSRAN_API int srsran_dci_ul_nr_to_str(const srsran_dci_ul_nr_t* dci, char* str, uint32_t str_len);

SRSRAN_API int srsran_dci_dl_nr_to_str(const srsran_dci_dl_nr_t* dci, char* str, uint32_t str_len);

#endif // SRSRAN_DCI_NR_H
