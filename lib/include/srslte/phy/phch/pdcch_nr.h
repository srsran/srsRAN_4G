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
#include "srslte/phy/ch_estimation/dmrs_pdcch.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/polar/polar_code.h"
#include "srslte/phy/fec/polar/polar_decoder.h"
#include "srslte/phy/fec/polar/polar_encoder.h"
#include "srslte/phy/fec/polar/polar_rm.h"
#include "srslte/phy/modem/evm.h"
#include "srslte/phy/modem/modem_table.h"

/**
 * @brief PDCCH configuration initialization arguments
 */
typedef struct {
  bool disable_simd;
  bool measure_evm;
  bool measure_time;
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
  uint8_t*               c;         // Message bits with attached CRC
  uint8_t*               d;         // encoded bits
  uint8_t*               f;         // bits at the Rate matching output
  uint8_t*               allocated; // Allocated polar bit buffer, encoder input, decoder output
  cf_t*                  symbols;
  srslte_modem_table_t   modem_table;
  srslte_evm_buffer_t*   evm_buffer;
  bool                   meas_time_en;
  uint32_t               meas_time_us;
  uint32_t               K;
  uint32_t               M;
  uint32_t               E;
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

SRSLTE_API int srslte_pdcch_nr_max_candidates_coreset(const srslte_coreset_t* coreset, uint32_t aggregation_level);

SRSLTE_API int srslte_pdcch_nr_init_tx(srslte_pdcch_nr_t* q, const srslte_pdcch_nr_args_t* args);

SRSLTE_API int srslte_pdcch_nr_init_rx(srslte_pdcch_nr_t* q, const srslte_pdcch_nr_args_t* args);

SRSLTE_API void srslte_pdcch_nr_free(srslte_pdcch_nr_t* q);

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

/**
 * @brief Stringifies NR PDCCH decoding information from the latest encoded/decoded transmission
 *
 * @param[in] q provides PDCCH encoder/decoder object
 * @param[in] res Optional PDCCH decode result
 * @param[out] str Destination string
 * @param[out] str_len Maximum destination string length
 * @return The number of characters written in the string
 */
SRSLTE_API uint32_t srslte_pdcch_nr_info(const srslte_pdcch_nr_t*     q,
                                         const srslte_pdcch_nr_res_t* res,
                                         char*                        str,
                                         uint32_t                     str_len);

#endif // SRSLTE_PDCCH_NR_H
