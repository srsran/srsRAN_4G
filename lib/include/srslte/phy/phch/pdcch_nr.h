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

/******************************************************************************
 *  File:         pdcch.h
 *
 *  Description:  Physical downlink control channel for NR.
 *
 *  Reference:    3GPP TS 38.211 version 15.10.0
 *****************************************************************************/

#ifndef SRSLTE_PDCCH_NR_H
#define SRSLTE_PDCCH_NR_H

#include "dci_nr.h"
#include "srslte/config.h"
#include "srslte/phy/ch_estimation/dmrs_pdcch.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/polar/polar_code.h"
#include "srslte/phy/fec/polar/polar_decoder.h"
#include "srslte/phy/fec/polar/polar_encoder.h"
#include "srslte/phy/fec/polar/polar_rm.h"

/**
 * @brief PDCCH configuration initialization arguments
 */
typedef struct {
  bool disable_simd;
  bool measure_evm;
} srslte_pdcch_nr_args_t;

/**
 * @brief PDCCH Attributes and objects required to encode/decode NR PDCCH
 */
typedef struct SRSLTE_API {
  bool                   is_tx;
  srslte_polar_code_t    code;
  srslte_polar_encoder_t encoder;
  srslte_polar_decoder_t decoder;
  srslte_polar_rm_t      rm;
  srslte_carrier_nr_t    carrier;
  srslte_coreset_t       coreset;
  srslte_crc_t           crc24c;
  uint8_t*               c; // Message bits with attached CRC
  uint8_t*               d; // encoded bits
  uint8_t*               f; // bits at the Rate matching output
  cf_t*                  symbols;
  srslte_modem_table_t   modem_table;
  srslte_evm_buffer_t*   evm_buffer;
} srslte_pdcch_nr_t;

/**
 * @brief NR PDCCH decoder result
 */
typedef struct SRSLTE_API {
  float evm;
  bool  crc;
} srslte_pdcch_nr_res_t;

/**
 * @brief Function for generating NR PDCCH candidate locations n_cce for a given CORESET, search space, aggregation
 * level and slot.
 *
 * @see srslte_pdcch_ue_locations_ncce
 * @see srslte_pdcch_common_locations_ncce
 *
 * @param coreset is the coreset configuration provided from higher layers
 * @param search_space is the Search Space configuration provided from higher layers
 * @param RNTI UE temporal identifier, unused for common search spaces
 * @param aggregation_level aggregation level in logarithm range (0,1,2,3,4)
 * @param slot_idx Slot index within the radio frame
 * @param locations is the destination array with the possible candidate locations n_cce
 */
int srslte_pdcch_nr_locations_coreset(const srslte_coreset_t*      coreset,
                                      const srslte_search_space_t* search_space,
                                      uint16_t                     rnti,
                                      uint32_t                     aggregation_level,
                                      uint32_t                     slot_idx,
                                      uint32_t locations[SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR]);

SRSLTE_API int srslte_pdcch_nr_init_tx(srslte_pdcch_nr_t* q, const srslte_pdcch_nr_args_t* args);

SRSLTE_API int srslte_pdcch_nr_init_rx(srslte_pdcch_nr_t* q, const srslte_pdcch_nr_args_t* args);

SRSLTE_API void srslte_pdcch_nr_init_free(srslte_pdcch_nr_t* q);

SRSLTE_API int
srslte_pdcch_nr_set_carrier(srslte_pdcch_nr_t* q, const srslte_carrier_nr_t* carrier, const srslte_coreset_t* coreset);

SRSLTE_API int srslte_pdcch_nr_encode(srslte_pdcch_nr_t* q, const srslte_dci_msg_nr_t* dci_msg, cf_t* slot_symbols);

/**
 * @brief Decodes a DCI
 *
 * @param[in,out] q provides PDCCH encoder/decoder object
 * @param[in] slot_symbols provides slot resource grid
 * @param[in] ce provides channel estimated resource elements
 * @param[in,out] dci_msg Provides with the DCI message location, RNTI, RNTI type and so on. Also, the message data
 * buffer
 * @param[out] res Provides the PDCCH result information
 * @return SRSLTE_SUCCESS if the configurations are valid, otherwise it returns an SRSLTE_ERROR code
 */
SRSLTE_API int srslte_pdcch_nr_decode(srslte_pdcch_nr_t*      q,
                                      cf_t*                   slot_symbols,
                                      srslte_dmrs_pdcch_ce_t* ce,
                                      srslte_dci_msg_nr_t*    dci_msg,
                                      srslte_pdcch_nr_res_t*  res);

#endif // SRSLTE_PDCCH_NR_H
