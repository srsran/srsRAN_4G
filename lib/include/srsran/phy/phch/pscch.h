/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_PSCCH_H
#define SRSRAN_PSCCH_H

#include <stdint.h>

#include "srsran/phy/common/phy_common_sl.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/dft/dft_precoding.h"
#include "srsran/phy/fec/convolutional/convcoder.h"
#include "srsran/phy/fec/convolutional/viterbi.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/modem/modem_table.h"

/**
 *  \brief Physical Sidelink control channel.
 *
 *  Reference: 3GPP TS 36.211 version 15.6.0 Release 15 Section 9.4
 */

typedef struct SRSRAN_API {

  uint32_t         max_prb;
  srsran_cell_sl_t cell;

  uint32_t sci_len;
  uint32_t nof_tx_re;

  uint32_t pscch_nof_prb;

  // crc
  uint8_t*     c;
  srsran_crc_t crc;
  uint8_t*     sci_crc;

  // channel coding
  srsran_viterbi_t   dec;
  srsran_convcoder_t encoder;
  uint8_t*           d;
  int16_t*           d_16;

  // rate matching
  uint32_t E;
  uint8_t* e;
  int16_t* e_16;

  uint8_t* e_bytes; ///< To pack bits to bytes
  uint32_t nof_symbols;

  // interleaving
  uint32_t* interleaver_lut;
  uint8_t*  codeword;
  uint8_t*  codeword_bytes;

  // scrambling
  srsran_sequence_t seq;

  // modulation
  srsran_modem_table_t mod;
  cf_t*                mod_symbols;
  int16_t*             llr;

  // dft precoding
  srsran_dft_precoding_t dft_precoder;
  srsran_dft_precoding_t idft_precoder;

  cf_t* scfdma_symbols;

} srsran_pscch_t;

SRSRAN_API int  srsran_pscch_init(srsran_pscch_t* q, uint32_t max_prb);
SRSRAN_API int  srsran_pscch_set_cell(srsran_pscch_t* q, srsran_cell_sl_t cell);
SRSRAN_API int  srsran_pscch_encode(srsran_pscch_t* q, uint8_t* sci, cf_t* sf_buffer, uint32_t prb_start_idx);
SRSRAN_API int  srsran_pscch_decode(srsran_pscch_t* q, cf_t* equalized_sf_syms, uint8_t* sci, uint32_t prb_start_idx);
SRSRAN_API int  srsran_pscch_put(srsran_pscch_t* q, cf_t* sf_buffer, uint32_t prb_start_idx);
SRSRAN_API int  srsran_pscch_get(srsran_pscch_t* q, cf_t* sf_buffer, uint32_t prb_start_idx);
SRSRAN_API void srsran_pscch_free(srsran_pscch_t* q);

#endif // SRSRAN_PSCCH_H
