/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_NPBCH_H
#define SRSRAN_NPBCH_H

#include "srsran/config.h"
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

#define SRSRAN_MIB_NB_LEN 34
#define SRSRAN_MIB_NB_CRC_LEN (SRSRAN_MIB_NB_LEN + 16)
#define SRSRAN_MIB_NB_ENC_LEN (3 * SRSRAN_MIB_NB_CRC_LEN)

#define SRSRAN_NPBCH_NUM_RE (12 * 11 - 4 * 8) // 100 RE, entire PRB minus 3 symbols minus 4 times NRS=CRS REs
#define SRSRAN_NPBCH_NUM_BLOCKS 8             // MIB-NB is split across 8 blocks
#define SRSRAN_NPBCH_NUM_REP 8                // number of repetitions per block
#define SRSRAN_NPBCH_NUM_FRAMES (SRSRAN_NPBCH_NUM_BLOCKS * SRSRAN_NPBCH_NUM_REP)

typedef struct {
  uint16_t            sfn;
  uint16_t            hfn;
  uint8_t             sched_info_sib1;
  uint8_t             sys_info_tag;
  bool                ac_barring;
  srsran_nbiot_mode_t mode;
} srsran_mib_nb_t;

/**
 * \brief Narrowband Physical broadcast channel (NPBCH)
 *
 *  Reference: 3GPP TS 36.211 version 13.2.0 Release 13 Sec. 10.2.4
 */
typedef struct SRSRAN_API {
  srsran_nbiot_cell_t cell;

  uint32_t frame_idx;
  uint32_t nof_symbols;

  // buffers
  cf_t*    ce[SRSRAN_MAX_PORTS];
  cf_t*    symbols[SRSRAN_MAX_PORTS];
  cf_t*    x[SRSRAN_MAX_PORTS];
  cf_t*    d;
  float*   llr;
  float*   temp;
  float    rm_f[SRSRAN_MIB_NB_ENC_LEN];
  uint8_t* rm_b;
  uint8_t  data[SRSRAN_MIB_NB_CRC_LEN];
  uint8_t  data_enc[SRSRAN_MIB_NB_ENC_LEN];

  srsran_nbiot_mode_t op_mode;

  // tx & rx objects
  srsran_modem_table_t mod;
  srsran_sequence_t    seq;
  srsran_sequence_t    seq_r14[SRSRAN_NPBCH_NUM_BLOCKS];
  srsran_viterbi_t     decoder;
  srsran_crc_t         crc;
  srsran_convcoder_t   encoder;
  bool                 search_all_ports;
} srsran_npbch_t;

SRSRAN_API int srsran_npbch_init(srsran_npbch_t* q);

SRSRAN_API void srsran_npbch_free(srsran_npbch_t* q);

SRSRAN_API int srsran_npbch_set_cell(srsran_npbch_t* q, srsran_nbiot_cell_t cell);

SRSRAN_API void srsran_npbch_mib_pack(uint32_t sfn, uint32_t hfn, srsran_mib_nb_t mib, uint8_t* msg);

SRSRAN_API void srsran_npbch_mib_unpack(uint8_t* msg, srsran_mib_nb_t* mib);

SRSRAN_API void srsran_mib_nb_printf(FILE* stream, srsran_nbiot_cell_t cell, srsran_mib_nb_t* mib);

SRSRAN_API int srsran_npbch_put_subframe(srsran_npbch_t* q,
                                         uint8_t         bch_payload[SRSRAN_MIB_NB_LEN],
                                         cf_t*           sf[SRSRAN_MAX_PORTS],
                                         uint32_t        frame_idx);

SRSRAN_API int srsran_npbch_encode(srsran_npbch_t* q,
                                   uint8_t         bch_payload[SRSRAN_MIB_NB_LEN],
                                   cf_t*           sf[SRSRAN_MAX_PORTS],
                                   uint32_t        frame_idx);

int srsran_npbch_rotate(srsran_npbch_t* q,
                        uint32_t        nf,
                        cf_t*           input_signal,
                        cf_t*           output_signal,
                        int             num_samples,
                        bool            back);

SRSRAN_API int srsran_npbch_decode(srsran_npbch_t* q,
                                   cf_t*           slot1_symbols,
                                   cf_t*           ce[SRSRAN_MAX_PORTS],
                                   float           noise_estimate,
                                   uint8_t         bch_payload[SRSRAN_MIB_NB_LEN],
                                   uint32_t*       nof_tx_ports,
                                   int*            sfn_offset);

SRSRAN_API int srsran_npbch_decode_nf(srsran_npbch_t* q,
                                      cf_t*           slot1_symbols,
                                      cf_t*           ce[SRSRAN_MAX_PORTS],
                                      float           noise_estimate,
                                      uint8_t         bch_payload[SRSRAN_MIB_NB_LEN],
                                      uint32_t*       nof_tx_ports,
                                      int*            sfn_offset,
                                      int             nf);

SRSRAN_API void srsran_npbch_decode_reset(srsran_npbch_t* q);

SRSRAN_API int srsran_npbch_decode_frame(srsran_npbch_t* q,
                                         uint32_t        src,
                                         uint32_t        dst,
                                         uint32_t        n,
                                         uint32_t        nof_bits,
                                         uint32_t        nof_ports);

SRSRAN_API uint32_t srsran_npbch_crc_check(srsran_npbch_t* q, uint8_t* bits, uint32_t nof_ports);

SRSRAN_API void srsran_npbch_crc_set_mask(uint8_t* data, int nof_ports);

SRSRAN_API int srsran_npbch_cp(cf_t* input, cf_t* output, srsran_nbiot_cell_t cell, bool put);

#endif // SRSRAN_NPBCH_H
