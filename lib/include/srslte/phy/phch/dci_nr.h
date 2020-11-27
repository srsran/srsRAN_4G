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
#include "srslte/phy/phch/pdsch_cfg_nr.h"

typedef enum SRSLTE_API {
  srslte_rnti_type_c = 0,
  srslte_rnti_type_p,
  srslte_rnti_type_si,
  srslte_rnti_type_ra,
  srslte_rnti_type_tc,
} srslte_rnti_type_t;

typedef struct SRSLTE_API {
  srslte_dci_location_t      location;
  srslte_search_space_type_t search_space;
  uint8_t                    payload[32];
  srslte_rnti_type_t         rnti_type;
  uint32_t                   nof_bits;
  srslte_dci_format_nr_t     format;
  uint16_t                   rnti;
} srslte_dci_msg_nr_t;

typedef struct SRSLTE_API {
  srslte_rnti_type_t rnti_type;

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

SRSLTE_API SRSLTE_API int srslte_dci_nr_format_1_0_sizeof(const srslte_carrier_nr_t* carrier,
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

#endif // SRSLTE_DCI_NR_H
