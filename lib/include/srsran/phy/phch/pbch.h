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

/******************************************************************************
 *  File:         pbch.h
 *
 *  Description:  Physical broadcast channel. If cell.nof_ports = 0, the number
 *                of ports is blindly determined using the CRC of the received
 *                codeword for 1, 2 and 4 ports
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.6
 *****************************************************************************/

#ifndef SRSRAN_PBCH_H
#define SRSRAN_PBCH_H

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/fec/convolutional/convcoder.h"
#include "srsran/phy/fec/convolutional/rm_conv.h"
#include "srsran/phy/fec/convolutional/viterbi.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/scrambling/scrambling.h"

#define SRSRAN_BCH_PAYLOAD_LEN 24
#define SRSRAN_BCH_PAYLOADCRC_LEN (SRSRAN_BCH_PAYLOAD_LEN + 16)
#define SRSRAN_BCH_ENCODED_LEN 3 * (SRSRAN_BCH_PAYLOADCRC_LEN)

#define SRSRAN_PBCH_MAX_RE 256 // make it avx2-aligned

/* PBCH object */
typedef struct SRSRAN_API {
  srsran_cell_t cell;

  uint32_t nof_symbols;

  /* buffers */
  cf_t*    ce[SRSRAN_MAX_PORTS];
  cf_t*    symbols[SRSRAN_MAX_PORTS];
  cf_t*    x[SRSRAN_MAX_PORTS];
  cf_t*    d;
  float*   llr;
  float*   temp;
  float    rm_f[SRSRAN_BCH_ENCODED_LEN];
  uint8_t* rm_b;
  uint8_t  data[SRSRAN_BCH_PAYLOADCRC_LEN];
  uint8_t  data_enc[SRSRAN_BCH_ENCODED_LEN];

  uint32_t frame_idx;

  /* tx & rx objects */
  srsran_modem_table_t mod;
  srsran_sequence_t    seq;
  srsran_viterbi_t     decoder;
  srsran_crc_t         crc;
  srsran_convcoder_t   encoder;
  bool                 search_all_ports;

} srsran_pbch_t;

SRSRAN_API int srsran_pbch_init(srsran_pbch_t* q);

SRSRAN_API void srsran_pbch_free(srsran_pbch_t* q);

SRSRAN_API int srsran_pbch_set_cell(srsran_pbch_t* q, srsran_cell_t cell);

SRSRAN_API int srsran_pbch_decode(srsran_pbch_t*         q,
                                  srsran_chest_dl_res_t* channel,
                                  cf_t*                  sf_symbols[SRSRAN_MAX_PORTS],
                                  uint8_t                bch_payload[SRSRAN_BCH_PAYLOAD_LEN],
                                  uint32_t*              nof_tx_ports,
                                  int*                   sfn_offset);

SRSRAN_API int srsran_pbch_encode(srsran_pbch_t* q,
                                  uint8_t        bch_payload[SRSRAN_BCH_PAYLOAD_LEN],
                                  cf_t*          sf_symbols[SRSRAN_MAX_PORTS],
                                  uint32_t       frame_idx);

SRSRAN_API void srsran_pbch_decode_reset(srsran_pbch_t* q);

SRSRAN_API void srsran_pbch_mib_unpack(uint8_t* msg, srsran_cell_t* cell, uint32_t* sfn);

SRSRAN_API void srsran_pbch_mib_pack(srsran_cell_t* cell, uint32_t sfn, uint8_t* msg);

#endif // SRSRAN_PBCH_H
