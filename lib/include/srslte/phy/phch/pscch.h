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

#ifndef SRSLTE_PSCCH_H
#define SRSLTE_PSCCH_H

#include <stdint.h>

#include "srslte/phy/common/phy_common_sl.h"
#include "srslte/phy/common/sequence.h"
#include "srslte/phy/dft/dft_precoding.h"
#include "srslte/phy/fec/convcoder.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/viterbi.h"
#include "srslte/phy/modem/modem_table.h"

/**
 *  \brief Physical Sidelink control channel.
 *
 *  Reference: 3GPP TS 36.211 version 15.6.0 Release 15 Section 9.4
 */

typedef struct SRSLTE_API {

  uint32_t         max_prb;
  srslte_cell_sl_t cell;

  uint32_t sci_len;
  uint32_t nof_tx_re;

  uint32_t pscch_nof_prb;

  // crc
  uint8_t*     c;
  srslte_crc_t crc;
  uint8_t*     sci_crc;

  // channel coding
  srslte_viterbi_t   dec;
  srslte_convcoder_t encoder;
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
  srslte_sequence_t seq;

  // modulation
  srslte_modem_table_t mod;
  cf_t*                mod_symbols;
  int16_t*             llr;

  // dft precoding
  srslte_dft_precoding_t dft_precoder;
  srslte_dft_precoding_t idft_precoder;

  cf_t* scfdma_symbols;

} srslte_pscch_t;

SRSLTE_API int  srslte_pscch_init(srslte_pscch_t* q, uint32_t max_prb);
SRSLTE_API int  srslte_pscch_set_cell(srslte_pscch_t* q, srslte_cell_sl_t cell);
SRSLTE_API int  srslte_pscch_encode(srslte_pscch_t* q, uint8_t* sci, cf_t* sf_buffer, uint32_t prb_start_idx);
SRSLTE_API int  srslte_pscch_decode(srslte_pscch_t* q, cf_t* equalized_sf_syms, uint8_t* sci, uint32_t prb_start_idx);
SRSLTE_API int  srslte_pscch_put(srslte_pscch_t* q, cf_t* sf_buffer, uint32_t prb_start_idx);
SRSLTE_API int  srslte_pscch_get(srslte_pscch_t* q, cf_t* sf_buffer, uint32_t prb_start_idx);
SRSLTE_API void srslte_pscch_free(srslte_pscch_t* q);

#endif // SRSLTE_PSCCH_H
