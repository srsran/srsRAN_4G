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

#ifndef SRSLTE_PUCCH_CFG_NR_H
#define SRSLTE_PUCCH_CFG_NR_H

#include "srslte/config.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum SRSLTE_API {
  SRSLTE_PUCCH_NR_FORMAT_0 = 0,
  SRSLTE_PUCCH_NR_FORMAT_1,
  SRSLTE_PUCCH_NR_FORMAT_2,
  SRSLTE_PUCCH_NR_FORMAT_3,
  SRSLTE_PUCCH_NR_FORMAT_4,
  SRSLTE_PUCCH_NR_FORMAT_ERROR,
} srslte_pucch_nr_format_t;

typedef enum SRSLTE_API {
  SRSLTE_PUCCH_NR_GROUP_HOPPING_NEITHER = 0,
  SRSLTE_PUCCH_NR_GROUP_HOPPING_ENABLE,
  SRSLTE_PUCCH_NR_GROUP_HOPPING_DISABLE
} srslte_pucch_nr_group_hopping_t;

/**
 * @brief PUCCH Common configuration
 * @remark Defined in TS 38.331 PUCCH-ConfigCommon
 */
typedef struct SRSLTE_API {
  uint32_t                        resource_common; ///< Configures a set of cell-specific PUCCH resources/parameters
  srslte_pucch_nr_group_hopping_t group_hopping;   ///< Configuration of group and sequence hopping
  uint32_t hopping_id; ///< Cell-specific scrambling ID for group hopping and sequence hopping if enabled
  bool     hopping_id_present;
  float    p0_nominal; ///< Power control parameter P0 for PUCCH transmissions. Value in dBm. (-202..24)
} srslte_pucch_nr_common_cfg_t;

/**
 * @brief Generic PUCCH Resource configuration
 * @remark Defined in TS 38.331 PUCCH-Config
 */
typedef struct SRSLTE_API {
  //
  uint32_t starting_prb;
  bool     intra_slot_hopping;
  uint32_t second_hop_prb;

  // Common PUCCH-Resource parameters among all formats
  srslte_pucch_nr_format_t format;           ///< PUCCH format this configuration belongs
  uint32_t                 nof_symbols;      ///< Number of symbols
  uint32_t                 start_symbol_idx; ///< Starting symbol index
  double                   max_code_rate;    ///< Maximum code rate (0.08, 0.15, 0.25, 0.35, 0.45, 0.60, 0.80)
  bool                     enable_pi_bpsk;   ///< Enables PI-BPSK

  // Specific PUCCH-Resource
  uint32_t initial_cyclic_shift; ///< Used by formats 0, 1
  uint32_t time_domain_occ;      ///< Used by format 1
  uint32_t nof_prb;              ///< Used by formats 2, 3
  uint32_t occ_lenth;            ///< Spreading factor, used by format 4
  uint32_t occ_index;            ///< Used by format 4
} srslte_pucch_nr_resource_t;

/**
 * @brief Validates a PUCCH format 0 resource configuration provided by upper layers
 * @param resource Resource configuration to validate
 * @return SRSLTE_SUCCESS if valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_pucch_nr_format0_resource_valid(const srslte_pucch_nr_resource_t* resource);

/**
 * @brief Validates a PUCCH format 1 resource configuration provided by upper layers
 * @param resource Resource configuration to validate
 * @return SRSLTE_SUCCESS if valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_pucch_nr_format1_resource_valid(const srslte_pucch_nr_resource_t* resource);

/**
 * @brief Validates a PUCCH format 2 resource configuration provided by upper layers
 * @param resource Resource configuration to validate
 * @return SRSLTE_SUCCESS if valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_pucch_nr_format2_resource_valid(const srslte_pucch_nr_resource_t* resource);

#endif // SRSLTE_PUCCH_CFG_NR_H
