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

#ifndef SRSLTE_PSSCH_H
#define SRSLTE_PSSCH_H

#include "srslte/phy/common/phy_common_sl.h"
#include "srslte/phy/dft/dft_precoding.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/turbocoder.h"
#include "srslte/phy/fec/turbodecoder.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/scrambling/scrambling.h"

/**
 *  \brief Physical Sidelink shared channel.
 *
 *  Reference: 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.3
 */

// Redundancy version
static const uint8_t srslte_pssch_rv[4] = {0, 2, 3, 1};

typedef struct SRSLTE_API {
  uint32_t prb_start_idx; // PRB start idx to map RE from RIV
  uint32_t nof_prb;       // PSSCH nof_prbs, Length of continuous PRB to map RE (in the pool) from RIV
  uint32_t N_x_id;
  uint32_t mcs_idx;
  uint32_t rv_idx;
  uint32_t sf_idx; // PSSCH sf_idx
} srslte_pssch_cfg_t;

typedef struct SRSLTE_API {

  srslte_cell_sl_t               cell;
  srslte_sl_comm_resource_pool_t sl_comm_resource_pool;
  srslte_pssch_cfg_t             pssch_cfg;

  srslte_cbsegm_t cb_segm;

  uint32_t G;
  uint32_t E;
  uint32_t Qm;
  uint32_t nof_data_symbols;
  uint32_t nof_tx_symbols;

  uint32_t nof_data_re; // Number of RE considered during the channel mapping
  uint32_t nof_tx_re;   // Number of RE actually transmitted over the air (without last OFDM symbol)
  uint32_t sl_sch_tb_len;
  uint32_t scfdma_symbols_len;

  // data
  uint8_t* b;

  // crc
  uint8_t*     c_r;
  uint8_t*     c_r_bytes;
  uint8_t*     tb_crc_temp;
  srslte_crc_t tb_crc;
  uint8_t*     cb_crc_temp;
  srslte_crc_t cb_crc;

  // channel coding
  uint8_t*      d_r;
  int16_t*      d_r_16;
  srslte_tcod_t tcod;
  srslte_tdec_t tdec;

  // rate matching
  uint8_t* e_r;
  int16_t* e_r_16;
  uint8_t* buff_b;

  uint8_t* codeword;
  uint8_t* codeword_bytes;
  int16_t* llr;

  // interleaving
  uint8_t*  f;
  uint8_t*  f_bytes;
  int16_t*  f_16;
  uint32_t* interleaver_lut;

  // scrambling
  srslte_sequence_t scrambling_seq;

  // modulation
  srslte_mod_t         mod_idx;
  srslte_modem_table_t mod[SRSLTE_MOD_NITEMS];
  cf_t*                symbols;
  uint8_t*             bits_after_demod;
  uint8_t*             bytes_after_demod;

  // dft precoding
  srslte_dft_precoding_t dft_precoder;
  cf_t*                  scfdma_symbols;
  srslte_dft_precoding_t idft_precoder;

} srslte_pssch_t;

SRSLTE_API int
                srslte_pssch_init(srslte_pssch_t* q, srslte_cell_sl_t cell, srslte_sl_comm_resource_pool_t sl_comm_resource_pool);
SRSLTE_API int  srslte_pssch_set_cfg(srslte_pssch_t* q, srslte_pssch_cfg_t pssch_cfg);
SRSLTE_API int  srslte_pssch_encode(srslte_pssch_t* q, uint8_t* input, uint32_t input_len, cf_t* sf_buffer);
SRSLTE_API int  srslte_pssch_decode(srslte_pssch_t* q, cf_t* equalized_sf_syms, uint8_t* output, uint32_t output_len);
SRSLTE_API int  srslte_pssch_put(srslte_pssch_t* q, cf_t* sf_buffer, cf_t* symbols);
SRSLTE_API int  srslte_pssch_get(srslte_pssch_t* q, cf_t* sf_buffer, cf_t* symbols);
SRSLTE_API void srslte_pssch_free(srslte_pssch_t* q);

#endif // SRSLTE_PSSCH_H