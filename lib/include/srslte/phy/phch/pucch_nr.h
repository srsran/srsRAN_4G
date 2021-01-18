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
#include <srslte/srslte.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Maximum number of symbols (without DMRS) that NR-PUCCH format 1 can transmit
 */
#define SRSLTE_PUCCH_NR_FORMAT1_N_MAX 7

/**
 * @brief Maximum number of bit that NR-PUCCH format 1 can carry
 */
#define SRSLTE_PUCCH_NR_FORMAT1_MAX_NOF_BITS 2

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
SRSLTE_API int srslte_pucch_nr_init(srslte_pucch_nr_t* q);

/**
 * @brief Deallocates an NR-PUCCH encoder/decoder object
 * @param q Object
 */
SRSLTE_API void srslte_pucch_nr_free(srslte_pucch_nr_t* q);

/**
 * @brief Computes the NR-PUCCH group sequence
 * @remark Implemented according to TS 38.211 clause 6.3.2.2.1 Group and sequence hopping
 * @param[in] carrier Serving cell and UL BWP configuration
 * @param[in] cfg PUCCH common configuration
 * @param[out] u_ Group sequence (u)
 * @param[out] v_ Base sequence (v)
 * @return SRSLTE_SUCCESS if provide arguments are right, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_pucch_nr_group_sequence(const srslte_carrier_nr_t*          carrier,
                                              const srslte_pucch_nr_common_cfg_t* cfg,
                                              uint32_t*                           u_,
                                              uint32_t*                           v_);

/**
 * @brief Computes the NR alpha index (1-NRE)
 * @param[in] carrier Serving cell and UL BWP configuration
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] l OFDM Symbol, relative to the NR-PUCCH transmission start
 * @param[in] l_prime Initial OFDM symbol, relative to the transmission slot start
 * @param[in] m0 Initial cyclic shift
 * @param[in] m_cs Set to zero expect for format 0
 * @param[out] alpha_idx Computed alpha index
 * @return SRSLTE_SUCCESS if provide arguments are right, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_pucch_nr_alpha_idx(const srslte_carrier_nr_t*          carrier,
                                         const srslte_pucch_nr_common_cfg_t* cfg,
                                         const srslte_dl_slot_cfg_t*         slot,
                                         uint32_t                            l,
                                         uint32_t                            l_prime,
                                         uint32_t                            m0,
                                         uint32_t                            m_cs,
                                         uint32_t*                           alpha_idx);

/**
 * @brief Validates a PUCCH format 1 resource configuration provided by upper layers
 * @param resource Resource configuration to validate
 * @return SRSLTE_SUCCESS if valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_pucch_nr_format0_resource_valid(const srslte_pucch_nr_resource_format0_t* resource);

/**
 * @brief Encode and writes NR-PUCCH format 0 in the resource grid
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
SRSLTE_API int srslte_pucch_nr_format0_encode(const srslte_pucch_nr_t*            q,
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
SRSLTE_API int srslte_pucch_nr_format0_measure(const srslte_pucch_nr_t*            q,
                                               const srslte_carrier_nr_t*          carrier,
                                               const srslte_pucch_nr_common_cfg_t* cfg,
                                               const srslte_dl_slot_cfg_t*         slot,
                                               srslte_pucch_nr_resource_format0_t* resource,
                                               uint32_t                            m_cs,
                                               const cf_t*                         slot_symbols,
                                               srslte_pucch_nr_measure_t*          measure);

/**
 * @brief Validates a PUCCH format 1 resource configuration provided by upper layers
 * @param resource Resource configuration to validate
 * @return SRSLTE_SUCCESS if valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_pucch_nr_format1_resource_valid(const srslte_pucch_nr_resource_format1_t* resource);

/**
 * @brief Get NR-PUCCH orthogonal sequence w
 * @remark Defined by TS 38.211 Table 6.3.2.4.1-2: Orthogonal sequences ... for PUCCH format 1
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] n_pucch Number of PUCCH symbols
 * @param[in] i sequence index
 * @param m OFDM symbol index
 * @return Orthogonal sequence complex number if valid, NAN otherwise
 */
SRSLTE_API cf_t srslte_pucch_nr_format1_w(const srslte_pucch_nr_t* q, uint32_t n_pucch, uint32_t i, uint32_t m);

/**
 * @brief Encodes and puts NR-PUCCH format 1 in the resource grid
 * @remark Described in TS 38.211 clause 6.3.2.4 PUCCH format 1
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] carrier Carrier configuration
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] resource PUCCH format 1 resource
 * @param[in] b Bits to encode in the message
 * @param[in] nof_bits Number of bits to encode in the message
 * @param[out] slot_symbols Resource grid of the given slot
 * @return SRSLTE_SUCCESS if successful, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_pucch_nr_format1_encode(const srslte_pucch_nr_t*                  q,
                                              const srslte_carrier_nr_t*                carrier,
                                              const srslte_pucch_nr_common_cfg_t*       cfg,
                                              const srslte_dl_slot_cfg_t*               slot,
                                              const srslte_pucch_nr_resource_format1_t* resource,
                                              uint8_t*                                  b,
                                              uint32_t                                  nof_bits,
                                              cf_t*                                     slot_symbols);

/**
 * @brief Decodes NR-PUCCH format 1
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] carrier Carrier configuration
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] resource PUCCH format 1 resource
 * @param[in] chest_res Channel estimator result
 * @param[in] slot_symbols Resource grid of the given slot
 * @param[out] b Bits to decode
 * @param[in] nof_bits Number of bits to decode in the message
 * @return SRSLTE_SUCCESS if successful, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_pucch_nr_format1_decode(srslte_pucch_nr_t*                        q,
                                              const srslte_carrier_nr_t*                carrier,
                                              const srslte_pucch_nr_common_cfg_t*       cfg,
                                              const srslte_dl_slot_cfg_t*               slot,
                                              const srslte_pucch_nr_resource_format1_t* resource,
                                              srslte_chest_ul_res_t*                    chest_res,
                                              cf_t*                                     slot_symbols,
                                              uint8_t  b[SRSLTE_PUCCH_NR_FORMAT1_MAX_NOF_BITS],
                                              uint32_t nof_bits);

#endif // SRSLTE_PUCCH_NR_H
