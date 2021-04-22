/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_PSBCH_H
#define SRSRAN_PSBCH_H

#include "srsran/phy/common/phy_common_sl.h"
#include "srsran/phy/dft/dft_precoding.h"
#include "srsran/phy/fec/convolutional/convcoder.h"
#include "srsran/phy/fec/convolutional/viterbi.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/modem_table.h"
#include "srsran/phy/scrambling/scrambling.h"

#define SRSRAN_SL_BCH_CRC_LEN 16

/**
 *  \brief Physical Sidelink broadcast channel.
 *
 *  Reference: 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.6
 */
typedef struct SRSRAN_API {
  uint32_t       N_sl_id;
  srsran_sl_tm_t tm;
  srsran_cp_t    cp;

  uint32_t nof_data_re; ///< Number of RE considered during the channel mapping
  uint32_t nof_tx_re;   ///< Number of RE actually transmitted over the air (without last OFDM symbol)
  uint32_t E;
  uint32_t Qm;
  uint32_t nof_prb;
  uint32_t nof_data_symbols;
  uint32_t nof_tx_symbols;
  uint32_t sl_bch_tb_len;
  uint32_t sl_bch_tb_crc_len;
  uint32_t sl_bch_encoded_len;
  float    precoding_scaling;

  // data
  uint8_t* c;

  // crc
  srsran_crc_t crc_mib_sl;
  uint8_t*     crc_temp;

  // channel coding
  srsran_viterbi_t   dec;
  srsran_convcoder_t encoder;
  uint8_t*           d;
  int16_t*           d_16;

  // rate matching
  uint8_t* e;
  uint8_t* e_bytes; ///< To pack bits to bytes
  int16_t* e_16;

  uint8_t* codeword;
  uint8_t* codeword_bytes;
  int16_t* llr;

  // interleaving
  uint32_t* interleaver_lut;

  // scrambling
  srsran_sequence_t seq;

  // modulation
  srsran_modem_table_t mod;
  cf_t*                mod_symbols;

  // dft precoding
  srsran_dft_precoding_t dft_precoder;
  srsran_dft_precoding_t idft_precoder;
  cf_t*                  scfdma_symbols;

} srsran_psbch_t;

SRSRAN_API int
srsran_psbch_init(srsran_psbch_t* q, uint32_t nof_prb, uint32_t N_sl_id, srsran_sl_tm_t tm, srsran_cp_t cp);

SRSRAN_API void srsran_psbch_free(srsran_psbch_t* q);

SRSRAN_API int srsran_psbch_encode(srsran_psbch_t* q, uint8_t* input, uint32_t input_len, cf_t* sf_buffer);

SRSRAN_API int srsran_psbch_decode(srsran_psbch_t* q, cf_t* scfdma_symbols, uint8_t* output, uint32_t max_output_len);

SRSRAN_API int srsran_psbch_reset(srsran_psbch_t* q, uint32_t N_sl_id);

SRSRAN_API int srsran_psbch_put(srsran_psbch_t* q, cf_t* symbols, cf_t* sf_buffer);

SRSRAN_API int srsran_psbch_get(srsran_psbch_t* q, cf_t* sf_buffer, cf_t* symbols);

#endif // SRSRAN_PSBCH_H
