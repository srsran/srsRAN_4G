/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#ifndef PBCH_
#define PBCH_

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/mimo/precoding.h"
#include "srslte/mimo/layermap.h"
#include "srslte/modem/mod.h"
#include "srslte/modem/demod_soft.h"
#include "srslte/scrambling/scrambling.h"
#include "srslte/fec/rm_conv.h"
#include "srslte/fec/convcoder.h"
#include "srslte/fec/viterbi.h"
#include "srslte/fec/crc.h"

#define BCH_PAYLOAD_LEN   24
#define BCH_PAYLOADCRC_LEN  (BCH_PAYLOAD_LEN+16)
#define BCH_ENCODED_LEN   3*(BCH_PAYLOADCRC_LEN)

#define PBCH_RE_SRSLTE_SRSLTE_CP_NORM    240
#define PBCH_RE_SRSLTE_SRSLTE_CP_EXT     216

typedef _Complex float cf_t;

/* PBCH object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;
  
  uint32_t nof_symbols;

  /* buffers */
  cf_t *ce[SRSLTE_MAX_PORTS];
  cf_t *pbch_symbols[SRSLTE_MAX_PORTS];
  cf_t *pbch_x[SRSLTE_MAX_PORTS];
  cf_t *pbch_d;
  float *pbch_llr;
  float *temp;
  float pbch_rm_f[BCH_ENCODED_LEN];
  uint8_t *pbch_rm_b;
  uint8_t data[BCH_PAYLOADCRC_LEN];
  uint8_t data_enc[BCH_ENCODED_LEN];

  uint32_t frame_idx;

  /* tx & rx objects */
  modem_table_t mod;
  demod_soft_t demod;
  sequence_t seq_pbch;
  viterbi_t decoder;
  crc_t crc;
  convcoder_t encoder;
  precoding_t precoding; 
  
} pbch_t;

SRSLTE_API int pbch_init(pbch_t *q,
                         srslte_cell_t cell);

SRSLTE_API void pbch_free(pbch_t *q);
SRSLTE_API int pbch_decode(pbch_t *q, 
                           cf_t *slot1_symbols, 
                           cf_t *ce_slot1[SRSLTE_MAX_PORTS], 
                           float noise_estimate, 
                           uint8_t bch_payload[BCH_PAYLOAD_LEN], 
                           uint32_t *nof_tx_ports,
                           uint32_t *sfn_offset);

SRSLTE_API int pbch_encode(pbch_t *q, 
                           uint8_t bch_payload[BCH_PAYLOAD_LEN], 
                           cf_t *slot1_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API void pbch_decode_reset(pbch_t *q);

SRSLTE_API void pbch_mib_unpack(uint8_t *msg, 
                                srslte_cell_t *cell, 
                                uint32_t *sfn);

SRSLTE_API void pbch_mib_pack(srslte_cell_t *cell, 
                              uint32_t sfn, 
                              uint8_t *msg);

SRSLTE_API void pbch_mib_fprint(FILE *stream, 
                                srslte_cell_t *cell, 
                                uint32_t sfn, 
                                uint32_t cell_id);

#endif // PBCH_
