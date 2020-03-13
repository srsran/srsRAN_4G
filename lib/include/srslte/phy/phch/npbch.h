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

#ifndef SRSLTE_NPBCH_H
#define SRSLTE_NPBCH_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"

#include "srslte/phy/fec/convcoder.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/rm_conv.h"
#include "srslte/phy/fec/viterbi.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/scrambling/scrambling.h"

#define SRSLTE_MIB_NB_LEN 34
#define SRSLTE_MIB_NB_CRC_LEN (SRSLTE_MIB_NB_LEN + 16)
#define SRSLTE_MIB_NB_ENC_LEN (3 * SRSLTE_MIB_NB_CRC_LEN)

#define SRSLTE_NPBCH_NUM_RE (12 * 11 - 4 * 8) // 100 RE, entire PRB minus 3 symbols minus 4 times NRS=CRS REs
#define SRSLTE_NPBCH_NUM_BLOCKS 8             // MIB-NB is split across 8 blocks
#define SRSLTE_NPBCH_NUM_REP 8                // number of repetitions per block
#define SRSLTE_NPBCH_NUM_FRAMES (SRSLTE_NPBCH_NUM_BLOCKS * SRSLTE_NPBCH_NUM_REP)

typedef struct {
  uint16_t            sfn;
  uint16_t            hfn;
  uint8_t             sched_info_sib1;
  uint8_t             sys_info_tag;
  bool                ac_barring;
  srslte_nbiot_mode_t mode;
} srslte_mib_nb_t;

/**
 * \brief Narrowband Physical broadcast channel (NPBCH)
 *
 *  Reference: 3GPP TS 36.211 version 13.2.0 Release 13 Sec. 10.2.4
 */
typedef struct SRS_API {
  srslte_nbiot_cell_t cell;

  uint32_t frame_idx;
  uint32_t nof_symbols;

  // buffers
  cf_t*    ce[SRSLTE_MAX_PORTS];
  cf_t*    symbols[SRSLTE_MAX_PORTS];
  cf_t*    x[SRSLTE_MAX_PORTS];
  cf_t*    d;
  float*   llr;
  float*   temp;
  float    rm_f[SRSLTE_MIB_NB_ENC_LEN];
  uint8_t* rm_b;
  uint8_t  data[SRSLTE_MIB_NB_CRC_LEN];
  uint8_t  data_enc[SRSLTE_MIB_NB_ENC_LEN];

  srslte_nbiot_mode_t op_mode;

  // tx & rx objects
  srslte_modem_table_t mod;
  srslte_sequence_t    seq;
  srslte_sequence_t    seq_r14[SRSLTE_NPBCH_NUM_BLOCKS];
  srslte_viterbi_t     decoder;
  srslte_crc_t         crc;
  srslte_convcoder_t   encoder;
  bool                 search_all_ports;
} srslte_npbch_t;

SRSLTE_API int srslte_npbch_init(srslte_npbch_t* q);

SRSLTE_API void srslte_npbch_free(srslte_npbch_t* q);

SRSLTE_API int srslte_npbch_set_cell(srslte_npbch_t* q, srslte_nbiot_cell_t cell);

SRSLTE_API void srslte_npbch_mib_pack(uint32_t sfn, uint32_t hfn, srslte_mib_nb_t mib, uint8_t* msg);

SRSLTE_API void srslte_npbch_mib_unpack(uint8_t* msg, srslte_mib_nb_t* mib);

SRSLTE_API void srslte_mib_nb_printf(FILE* stream, srslte_nbiot_cell_t cell, srslte_mib_nb_t* mib);

SRSLTE_API int srslte_npbch_put_subframe(srslte_npbch_t* q,
                                         uint8_t         bch_payload[SRSLTE_MIB_NB_LEN],
                                         cf_t*           sf[SRSLTE_MAX_PORTS],
                                         uint32_t        frame_idx);

SRSLTE_API int srslte_npbch_encode(srslte_npbch_t* q,
                                   uint8_t         bch_payload[SRSLTE_MIB_NB_LEN],
                                   cf_t*           sf[SRSLTE_MAX_PORTS],
                                   uint32_t        frame_idx);

int srslte_npbch_rotate(srslte_npbch_t* q,
                        uint32_t        nf,
                        cf_t*           input_signal,
                        cf_t*           output_signal,
                        int             num_samples,
                        bool            back);

SRSLTE_API int srslte_npbch_decode(srslte_npbch_t* q,
                                   cf_t*           slot1_symbols,
                                   cf_t*           ce[SRSLTE_MAX_PORTS],
                                   float           noise_estimate,
                                   uint8_t         bch_payload[SRSLTE_MIB_NB_LEN],
                                   uint32_t*       nof_tx_ports,
                                   int*            sfn_offset);

SRSLTE_API int srslte_npbch_decode_nf(srslte_npbch_t* q,
                                      cf_t*           slot1_symbols,
                                      cf_t*           ce[SRSLTE_MAX_PORTS],
                                      float           noise_estimate,
                                      uint8_t         bch_payload[SRSLTE_MIB_NB_LEN],
                                      uint32_t*       nof_tx_ports,
                                      int*            sfn_offset,
                                      int             nf);

SRSLTE_API void srslte_npbch_decode_reset(srslte_npbch_t* q);

SRSLTE_API int srslte_npbch_decode_frame(srslte_npbch_t* q,
                                         uint32_t        src,
                                         uint32_t        dst,
                                         uint32_t        n,
                                         uint32_t        nof_bits,
                                         uint32_t        nof_ports);

SRSLTE_API uint32_t srslte_npbch_crc_check(srslte_npbch_t* q, uint8_t* bits, uint32_t nof_ports);

SRSLTE_API void srslte_npbch_crc_set_mask(uint8_t* data, int nof_ports);

SRSLTE_API int srslte_npbch_cp(cf_t* input, cf_t* output, srslte_nbiot_cell_t cell, bool put);

#endif // SRSLTE_NPBCH_H
