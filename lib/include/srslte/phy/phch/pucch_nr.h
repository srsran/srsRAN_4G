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

#ifndef SRSLTE_PUCCH_NR_H
#define SRSLTE_PUCCH_NR_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/common/zc_sequence.h"
#include "srslte/phy/modem/modem_table.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Maximum number of NR-PUCCH format 1 symbols (without DMRS)
 */
#define SRSLTE_PUCCH_NR_FORMAT1_N_MAX 7

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
  bool     intra_slot_frequency_hopping;
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

typedef struct SRSLTE_API {
  float rsrp;
  float rsrp_dBfs;
  float epre;
  float epre_dBfs;
  float norm_corr;
} srslte_pucch_nr_measure_t;

/**
 * @brief PUCCH Resource configuration for Format 0
 * @remark Defined in TS 38.331 PUCCH-Config
 */
typedef struct SRSLTE_API {
  uint32_t starting_prb;
  uint32_t initial_cyclic_shift; ///< initialCyclicShift (0..11)
  uint32_t start_symbol_idx;     ///< startingSymbolIndex (0..13)
  uint32_t nof_symbols;          ///< nrofSymbols (1..2)
} srslte_pucch_nr_resource_format0_t;

/**
 * @brief PUCCH Resource configuration for Format 1
 * @remark Defined in TS 38.331 PUCCH-Config
 */
typedef struct SRSLTE_API {
  uint32_t starting_prb;
  uint32_t initial_cyclic_shift; ///< initialCyclicShift (0..11)
  uint32_t start_symbol_idx;     ///< startingSymbolIndex (0..10)
  uint32_t nof_symbols;          ///< nrofSymbols (4..14)
  uint32_t time_domain_occ;      ///< TimeDomainOCC(0..6)
  bool     intra_slot_hopping;
} srslte_pucch_nr_resource_format1_t;

/**
 * @brief NR-PUCCH encoder/decoder object
 */
typedef struct SRSLTE_API {
  srslte_zc_sequence_lut_t r_uv_1prb;
  cf_t format1_w_i_m[SRSLTE_PUCCH_NR_FORMAT1_N_MAX][SRSLTE_PUCCH_NR_FORMAT1_N_MAX][SRSLTE_PUCCH_NR_FORMAT1_N_MAX];
  srslte_modem_table_t bpsk;
  srslte_modem_table_t qpsk;
} srslte_pucch_nr_t;

/**
 * @brief Initialises an NR-PUCCH encoder/decoder object
 * @param q Object
 * @return SRSLTE_SUCCESS if successful, SRSLTE_ERROR code otherwise
 */
int srslte_pucch_nr_init(srslte_pucch_nr_t* q);

/**
 * @brief Deallocates an NR-PUCCH encoder/decoder object
 * @param q Object
 */
void srslte_pucch_nr_free(srslte_pucch_nr_t* q);

/**
 * @brief Puts NR-PUCCH format 0 in the resource grid
 * @remark Described in TS 38.211 clause 6.3.2.3 PUCCH format 0
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] carrier Carrier configuration
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] resource PUCCH format 0 resource
 * @param[in] m_cs Cyclic shift according to TS 38.213 clause 5
 * @param[out] slot_symbols Resource grid of the given slot
 * @return SRSLTE_SUCCESS if successful, SRSLTE_ERROR code otherwise
 */
int srslte_pucch_nr_format0_put(const srslte_pucch_nr_t*            q,
                                const srslte_carrier_nr_t*          carrier,
                                const srslte_pucch_nr_common_cfg_t* cfg,
                                const srslte_dl_slot_cfg_t*         slot,
                                srslte_pucch_nr_resource_format0_t* resource,
                                uint32_t                            m_cs,
                                cf_t*                               slot_symbols);

/**
 * @brief Measures PUCCH format 0 in the resource grid
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] carrier Carrier configuration
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] resource PUCCH format 0 resource
 * @param[in] m_cs Cyclic shift according to TS 38.213 clause 5
 * @param[in] slot_symbols Resource grid of the given slot
 * @param[out] measure Measurement structure
 * @return SRSLTE_SUCCESS if successful, SRSLTE_ERROR code otherwise
 */
int srslte_pucch_nr_format0_measure(const srslte_pucch_nr_t*            q,
                                    const srslte_carrier_nr_t*          carrier,
                                    const srslte_pucch_nr_common_cfg_t* cfg,
                                    const srslte_dl_slot_cfg_t*         slot,
                                    srslte_pucch_nr_resource_format0_t* resource,
                                    uint32_t                            m_cs,
                                    const cf_t*                         slot_symbols,
                                    srslte_pucch_nr_measure_t*          measure);

#endif // SRSLTE_PUCCH_NR_H
