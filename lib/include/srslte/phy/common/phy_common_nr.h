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

#ifndef SRSLTE_PHY_COMMON_NR_H
#define SRSLTE_PHY_COMMON_NR_H

#include "phy_common.h"
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the number of symbols per slot. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSLTE_NSYMB_PER_SLOT_NR 14

/**
 * @brief Defines the resource grid size in physical resource elements (frequency and time domain)
 */
#define SRSLTE_SLOT_LEN_RE_NR(nof_prb) (nof_prb * SRSLTE_NRE * SRSLTE_NSYMB_PER_SLOT_NR)

#define SRSLTE_SLOT_MAX_LEN_RE_NR (SRSLTE_SLOT_LEN_RE_NR(SRSLTE_MAX_PRB_NR))
#define SRSLTE_SLOT_MAX_NOF_BITS_NR (SRSLTE_SLOT_MAX_LEN_RE_NR * SRSLTE_MAX_QM)
#define SRSLTE_MAX_LAYERS_NR 8

/**
 * @brief Defines the maximum numerology supported. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSLTE_NR_MAX_NUMEROLOGY 4

/**
 * @brief Defines the symbol duration, including cyclic prefix
 */
#define SRSLTE_SUBC_SPACING_NR(NUM) (15000U << (NUM))

/**
 * @brief Defines the number of slots per SF. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSLTE_NSLOTS_PER_SF_NR(NUM) (1U << (NUM))

/**
 * @brief Defines the number of slots per frame. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSLTE_NSLOTS_PER_FRAME_NR(NUM) (SRSLTE_NSLOTS_PER_SF_NR(NUM) * SRSLTE_NOF_SF_X_FRAME)

/**
 * @brief Maximum Carrier identification value. Defined by TS 38.331 v15.10.0 as PhysCellId from 0 to 1007.
 */
#define SRSLTE_MAX_ID_NR 1007

/**
 * @brief Maximum number of physical resource blocks (PRB) that a 5G NR can have. This is defined by TS 38.331 v15.10.0
 * as maxNrofPhysicalResourceBlocks
 */
#define SRSLTE_MAX_PRB_NR 275

/**
 * @brief Maximum start sub-carrier index for the carrier relative point
 */
#define SRSLTE_MAX_START_NR 2199

/**
 * @brief defines the maximum number of Aggregation levels: 1, 2, 4, 8 and 16
 */
#define SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR 5

/**
 * @brief defines the maximum number of candidates for a given Aggregation level
 */
#define SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR 8

/**
 * @brief defines the maximum number of resource elements per PRB
 * @remark Defined in TS 36.214 V15.10.0 5.1.3.2 Transport block size determination, point 1, second bullet
 */
#define SRSLTE_MAX_NRE_NR 156

/**
 * @brief defines the maximum number of resource elements in a PDSCH transmission
 * @remark deduced from in TS 36.214 V15.10.0 5.1.3.2 Transport block size determination
 */
#define SRSLTE_PDSCH_MAX_RE_NR (SRSLTE_MAX_NRE_NR * SRSLTE_MAX_PRB_NR)

/**
 * @brief Maximum number of PDSCH time domain resource allocations. This is defined by TS 38.331 v15.10.0
 * as maxNrofDL-Allocations
 */
#define SRSLTE_MAX_NOF_DL_ALLOCATION 16

typedef enum SRSLTE_API {
  srslte_coreset_mapping_type_interleaved = 0,
  srslte_coreset_mapping_type_non_interleaved,
} srslte_coreset_mapping_type_t;

typedef enum SRSLTE_API {
  srslte_coreset_bundle_size_n2 = 0,
  srslte_coreset_bundle_size_n3,
  srslte_coreset_bundle_size_n6,
} srslte_coreset_bundle_size_t;

typedef enum SRSLTE_API {
  srslte_coreset_precoder_granularity_contiguous = 0,
  srslte_coreset_precoder_granularity_reg_bundle
} srslte_coreset_precoder_granularity_t;

/**
 * @brief PDSCH mapping type
 * @remark Described in TS 38.331 V15.10.0 Section PDSCH-TimeDomainResourceAllocationList
 */
typedef enum SRSLTE_API { srslte_pdsch_mapping_type_A = 0, srslte_pdsch_mapping_type_B } srslte_pdsch_mapping_type_t;

typedef enum SRSLTE_API {
  srslte_search_space_type_common = 0,
  srslte_search_space_type_ue,
} srslte_search_space_type_t;

/**
 * @brief Indicates the MCS table the UE shall use for PDSCH and/or PUSCH without transform precoding
 */
typedef enum SRSLTE_API {
  srslte_mcs_table_64qam = 0,
  srslte_mcs_table_256qam,
  srslte_mcs_table_qam64LowSE,
  srslte_mcs_table_N
} srslte_mcs_table_t;

/**
 * @brief DCI formats
 * @remark Described in TS 38.212 V15.9.0 Section 7.3.1 DCI formats
 */
typedef enum SRSLTE_API {
  srslte_dci_format_nr_0_0 = 0, ///< @brief Scheduling of PUSCH in one cell
  srslte_dci_format_nr_0_1,     ///< @brief Scheduling of PUSCH in one cell
  srslte_dci_format_nr_1_0,     ///< @brief Scheduling of PDSCH in one cell
  srslte_dci_format_nr_1_1,     ///< @brief Scheduling of PDSCH in one cell
  srslte_dci_format_nr_2_0,     ///< @brief Notifying a group of UEs of the slot format
  srslte_dci_format_nr_2_1, ///< @brief Notifying a group of UEs of the PRB(s) and OFDM symbol(s) where UE may assume no
                            ///< transmission is intended for the UE
  srslte_dci_format_nr_2_2, ///< @brief Transmission of TPC commands for PUCCH and PUSCH
  srslte_dci_format_nr_2_3  ///< @brief Transmission of a group of TPC commands for SRS transmissions by one or more UEs
} srslte_dci_format_nr_t;

/**
 * @brief Overhead configuration provided by higher layers
 * @remark Described in TS 38.331 V15.10.0 PDSCH-ServingCellConfig
 * @remark Described in TS 38.331 V15.10.0 PUSCH-ServingCellConfig
 */
typedef enum SRSLTE_API {
  srslte_xoverhead_0 = 0,
  srslte_xoverhead_6,
  srslte_xoverhead_12,
  srslte_xoverhead_18
} srslte_xoverhead_t;

/**
 * @brief NR carrier parameters. It is a combination of fixed cell and bandwidth-part (BWP)
 */
typedef struct SRSLTE_API {
  uint32_t id;
  uint32_t numerology;
  uint32_t nof_prb;
  uint32_t start;
} srslte_carrier_nr_t;

/**
 * @brief NR Slot parameters. It contains parameters that change in a slot basis.
 */
typedef struct SRSLTE_API {
  /// Slot index in the radio frame
  uint32_t idx;

  /// Left for future parameters
  /// ...
} srslte_dl_slot_cfg_t;

/**
 * @brief Min number of OFDM symbols in a control resource set.
 */
#define SRSLTE_CORESET_DURATION_MIN 1

/**
 * @brief Max number of OFDM symbols in a control resource set. Specified in TS 38.331 V15.10.0 as maxCoReSetDuration
 */
#define SRSLTE_CORESET_DURATION_MAX 3

/**
 * @brief Number of possible CORESET frequency resources.
 */
#define SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE 45

/**
 * @brief Max value for shift index
 */
#define SRSLTE_CORESET_SHIFT_INDEX_MAX (SRSLTE_CORESET_NOF_PRB_MAX - 1)

/**
 * @brief CORESET parameters as defined in TS 38.331 V15.10.0 - ControlResourceSet
 */
typedef struct SRSLTE_API {
  srslte_coreset_mapping_type_t mapping_type;
  uint32_t                      id;
  uint32_t                      duration;
  bool                          freq_resources[SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE];
  srslte_coreset_bundle_size_t  interleaver_size;

  bool                                  dmrs_scrambling_id_present;
  uint32_t                              dmrs_scrambling_id;
  srslte_coreset_precoder_granularity_t precoder_granularity;
  srslte_coreset_bundle_size_t          reg_bundle_size;
  uint32_t                              shift_index;
  /** Missing TCI parameters */
} srslte_coreset_t;

/**
 * @brief SearchSpace parameters as defined in TS 38.331 v15.10.0 SearchSpace sequence
 */
typedef struct SRSLTE_API {
  uint32_t                   id;
  uint32_t                   duration; // in slots
  srslte_search_space_type_t type;
  uint32_t                   nof_candidates[SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR];
} srslte_search_space_t;

/**
 * @brief PDCCH configuration
 */
typedef struct SRSLTE_API {
  srslte_carrier_nr_t   carrier;
  uint16_t              rnti;
  srslte_coreset_t      coreset;
  srslte_search_space_t search_space;
  uint32_t              aggregation_level;
  uint32_t              n_cce;
} srslte_pdcch_cfg_nr_t;

/**
 * @brief Calculates the bandwidth of a given CORESET in physical resource blocks (PRB) . This function uses the
 * frequency domain resources bit-map for counting the number of PRB.
 *
 * @attention Prior to this function call, the frequency domain resources bit-map shall be zeroed beyond the
 * carrier.nof_prb / 6 index, otherwise the CORESET bandwidth might be greater than the carrier.
 *
 * @param coreset provides the given CORESET configuration
 * @return The number of PRB that the CORESET takes in frequency domain
 */
SRSLTE_API uint32_t srslte_coreset_get_bw(const srslte_coreset_t* coreset);

/**
 * @brief Calculates the number of Physical Resource Elements (time and frequency domain) that a given CORESET uses.
 * This function uses the frequency domain resources bit-map and duration to compute the number of symbols.
 *
 * @attention Prior to this function call, the frequency domain resources bit-map shall be zeroed beyond the
 * carrier.nof_prb / 6 index, otherwise the CORESET bandwidth might be greater than the carrier.
 *
 * @param coreset provides the given CORESET configuration
 * @return The number of resource elements that compose the coreset
 */
SRSLTE_API uint32_t srslte_coreset_get_sz(const srslte_coreset_t* coreset);

/**
 * @brief Get the MCS table string
 * @param mcs_table MCS table value
 * @return Constant string with the MCS table name
 */
SRSLTE_API const char* srslte_mcs_table_to_str(srslte_mcs_table_t mcs_table);

/**
 * @brief Get the MCS table value from a string
 * @param str Points to a given string
 * @return The MCS table value
 */
SRSLTE_API srslte_mcs_table_t srslte_mcs_table_from_str(const char* str);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_PHY_COMMON_NR_H
