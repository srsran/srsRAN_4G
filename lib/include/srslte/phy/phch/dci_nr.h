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

#ifndef SRSLTE_DCI_NR_H
#define SRSLTE_DCI_NR_H

#include "dci.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/phch/phch_cfg_nr.h"

typedef struct SRSLTE_API {
  srslte_dci_location_t      location;
  srslte_search_space_type_t search_space;
  uint32_t                   coreset_id;
  uint8_t                    payload[50];
  srslte_rnti_type_t         rnti_type;
  uint32_t                   nof_bits;
  srslte_dci_format_nr_t     format;
  uint16_t                   rnti;
} srslte_dci_msg_nr_t;

typedef struct SRSLTE_API {
  uint16_t                   rnti;
  srslte_rnti_type_t         rnti_type;
  srslte_dci_format_nr_t     format;
  srslte_dci_location_t      location;
  srslte_search_space_type_t search_space;
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

} srslte_dci_dl_nr_t;

typedef struct SRSLTE_API {
  // Context information
  uint16_t                   rnti;
  srslte_rnti_type_t         rnti_type;
  srslte_dci_format_nr_t     format;
  srslte_dci_location_t      location;
  srslte_search_space_type_t search_space;
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
} srslte_dci_ul_nr_t;

/**
 * @brief Indicates whether the provided DCI message format bit indicator belongs to DCI format 1_0 according according
 * to the RNTI type. If invalid, the DCI message is likely to be format 0_0
 * @param dci_msg Provides DCI format 1_0 message
 * @return true if the DCI message is format 1_0, false otherwise
 */
SRSLTE_API bool srslte_dci_nr_format_1_0_valid(const srslte_dci_msg_nr_t* dci_msg);

SRSLTE_API int srslte_dci_nr_pack(const srslte_carrier_nr_t* carrier,
                                  const srslte_coreset_t*    coreset,
                                  const srslte_dci_dl_nr_t*  dci,
                                  srslte_dci_msg_nr_t*       msg);

SRSLTE_API int srslte_dci_nr_format_0_0_sizeof(const srslte_carrier_nr_t* carrier,
                                               const srslte_coreset_t*    coreset,
                                               srslte_rnti_type_t         rnti_type);

SRSLTE_API int srslte_dci_nr_format_0_0_pack(const srslte_carrier_nr_t* carrier,
                                             const srslte_coreset_t*    coreset0,
                                             const srslte_dci_ul_nr_t*  dci,
                                             srslte_dci_msg_nr_t*       msg);

SRSLTE_API int srslte_dci_nr_format_0_0_unpack(const srslte_carrier_nr_t* carrier,
                                               const srslte_coreset_t*    coreset,
                                               srslte_dci_msg_nr_t*       msg,
                                               srslte_dci_ul_nr_t*        dci);

/**
 * @brief Unpacks DCI from Random Access Response Grant
 * @remark Described in TS 38.213 Table 8.2-1: Random Access Response Grant Content field size
 * @param msg
 * @param dci
 * @return SRSLTE_SUCCESS if unpacked correctly, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_dci_nr_rar_unpack(srslte_dci_msg_nr_t* msg, srslte_dci_ul_nr_t* dci);

SRSLTE_API int srslte_dci_nr_format_1_0_sizeof(const srslte_carrier_nr_t* carrier,
                                               const srslte_coreset_t*    coreset,
                                               srslte_rnti_type_t         rnti_type);

SRSLTE_API int srslte_dci_nr_format_1_0_pack(const srslte_carrier_nr_t* carrier,
                                             const srslte_coreset_t*    coreset,
                                             const srslte_dci_dl_nr_t*  dci,
                                             srslte_dci_msg_nr_t*       msg);

SRSLTE_API int srslte_dci_nr_format_1_0_unpack(const srslte_carrier_nr_t* carrier,
                                               const srslte_coreset_t*    coreset,
                                               srslte_dci_msg_nr_t*       msg,
                                               srslte_dci_dl_nr_t*        dci);

SRSLTE_API int srslte_dci_ul_nr_to_str(const srslte_dci_ul_nr_t* dci, char* str, uint32_t str_len);

SRSLTE_API int srslte_dci_dl_nr_to_str(const srslte_dci_dl_nr_t* dci, char* str, uint32_t str_len);

#endif // SRSLTE_DCI_NR_H
