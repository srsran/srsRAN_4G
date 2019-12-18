/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSLTE_PSBCH_H
#define SRSLTE_PSBCH_H

#include <srslte/phy/dft/dft_precoding.h>
#include <srslte/phy/fec/convcoder.h>
#include <srslte/phy/fec/crc.h>
#include <srslte/phy/fec/viterbi.h>
#include <srslte/phy/modem/demod_soft.h>
#include <srslte/phy/modem/modem_table.h>
#include <srslte/phy/scrambling/scrambling.h>

#define SRSLTE_SL_BCH_CRC_LEN 16
#define SRSLTE_SL_BCH_PAYLOAD_LEN 40
#define SRSLTE_SL_BCH_PAYLOADCRC_LEN (SRSLTE_SL_BCH_PAYLOAD_LEN + SRSLTE_SL_BCH_CRC_LEN)
#define SRSLTE_SL_BCH_ENCODED_LEN 3 * (SRSLTE_SL_BCH_PAYLOADCRC_LEN)

typedef struct {
  bool     is_ue;
  uint32_t E;
  uint32_t Qm;
  uint32_t len_after_mod;
  uint32_t nof_prb;
  uint32_t nof_symbols;
  uint32_t N_sl_id;
  float    precoding_scaling;
  uint32_t nof_prb_psbch;

  // data
  uint8_t* a;

  // crc
  uint32_t     crc_poly;
  srslte_crc_t crc_mib;
  uint8_t*     crc_temp;

  // channel coding
  srslte_viterbi_t   dec;
  srslte_convcoder_t encoder;
  uint8_t*           d;

  // rate matching
  uint8_t* e;
  int16_t* e_16;

  // interleaving
  uint32_t* interleaver_lut;
  uint8_t*  codeword;

  // scrambling
  srslte_sequence_t seq;

  // modulation
  srslte_modem_table_t mod;
  cf_t*                symbols;
  float*               llr;
  // layer mapping

  // dft precoding
  srslte_dft_precoding_t dft_precoder;
  srslte_dft_precoding_t idft_precoder;
  cf_t*                  scfdma_symbols;

} srslte_psbch_t;

int srslte_psbch_init(srslte_psbch_t* q, uint32_t N_sl_id, uint32_t nof_prb);

void srslte_psbch_encode(srslte_psbch_t* q, uint8_t* mib_sl);
int  srslte_psbch_decode(srslte_psbch_t* q, uint8_t* mib_sl);

void srslte_psbch_put(srslte_psbch_t* q, cf_t* sf_buffer);
void srslte_psbch_get(srslte_psbch_t* q, cf_t* sf_buffer);

void srslte_psbch_free(srslte_psbch_t* q);

#endif // SRSLTE_PSBCH_H
