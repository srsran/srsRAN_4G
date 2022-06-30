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

#ifndef SRSRAN_NPDSCH_H
#define SRSRAN_NPDSCH_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/fec/convolutional/convcoder.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/npdsch_cfg.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/phch/sch.h"
#include "srsran/phy/scrambling/scrambling.h"

#define SRSRAN_NPDSCH_MAX_RE                                                                                           \
  (SRSRAN_CP_NORM_SF_NSYMB * SRSRAN_NRE - 8) ///< Full PRB minus 8 RE for NRS (one antenna port)
#define SRSRAN_NPDSCH_MAX_TBS 680            ///< Max TBS in Rel13 NB-IoT
#define SRSRAN_NPDSCH_CRC_LEN (24)
#define SRSRAN_NPDSCH_MAX_TBS_CRC (SRSRAN_NPDSCH_MAX_TBS + SRSRAN_NPDSCH_CRC_LEN)
#define SRSRAN_NPDSCH_MAX_TBS_ENC (3 * SRSRAN_NPDSCH_MAX_TBS_CRC)
#define SRSRAN_NPDSCH_MAX_NOF_SF 10
#define SRSRAN_NPDSCH_NUM_SEQ (2 * SRSRAN_NOF_SF_X_FRAME) ///< for even and odd numbered SFNs

/* @brief Narrowband Physical Downlink shared channel (NPDSCH)
 *
 * Reference:    3GPP TS 36.211 version 13.2.0 Release 13 Sec. 10.2.3
 */
typedef struct SRSRAN_API {
  srsran_nbiot_cell_t cell;
  uint32_t            max_re;
  bool                rnti_is_set;
  uint16_t            rnti;

  // buffers
  uint8_t data[SRSRAN_NPDSCH_MAX_TBS_CRC];
  uint8_t data_enc[SRSRAN_NPDSCH_MAX_TBS_ENC];
  float   rm_f[SRSRAN_NPDSCH_MAX_TBS_ENC];
  cf_t*   ce[SRSRAN_MAX_PORTS];
  cf_t*   symbols[SRSRAN_MAX_PORTS];
  cf_t*   sib_symbols[SRSRAN_MAX_PORTS]; // extra buffer for SIB1 symbols as they may be interleaved with other NPDSCH
  cf_t*   tx_syms[SRSRAN_MAX_PORTS];     // pointer to either symbols or sib1_symbols
  cf_t*   x[SRSRAN_MAX_PORTS];
  cf_t*   d;

  float*   llr;
  uint8_t* temp;
  uint8_t* rm_b;

  // tx & rx objects
  srsran_modem_table_t mod;
  srsran_viterbi_t     decoder;
  srsran_sequence_t    seq[SRSRAN_NPDSCH_NUM_SEQ];
  srsran_crc_t         crc;
  srsran_convcoder_t   encoder;
} srsran_npdsch_t;

typedef struct {
  uint16_t hyper_sfn;
  // TODO: add all other fields
} srsran_sys_info_block_type_1_nb_t;

SRSRAN_API int srsran_npdsch_init(srsran_npdsch_t* q);

SRSRAN_API void srsran_npdsch_free(srsran_npdsch_t* q);

SRSRAN_API int srsran_npdsch_set_cell(srsran_npdsch_t* q, srsran_nbiot_cell_t cell);

SRSRAN_API int srsran_npdsch_set_rnti(srsran_npdsch_t* q, uint16_t rnti);

SRSRAN_API int srsran_npdsch_cfg(srsran_npdsch_cfg_t*        cfg,
                                 srsran_nbiot_cell_t         cell,
                                 srsran_ra_nbiot_dl_grant_t* grant,
                                 uint32_t                    sf_idx);

SRSRAN_API int srsran_npdsch_encode(srsran_npdsch_t*        q,
                                    srsran_npdsch_cfg_t*    cfg,
                                    srsran_softbuffer_tx_t* softbuffer,
                                    uint8_t*                data,
                                    cf_t*                   sf_symbols[SRSRAN_MAX_PORTS]);

SRSRAN_API int srsran_npdsch_encode_rnti_idx(srsran_npdsch_t*        q,
                                             srsran_npdsch_cfg_t*    cfg,
                                             srsran_softbuffer_tx_t* softbuffer,
                                             uint8_t*                data,
                                             uint32_t                rnti_idx,
                                             cf_t*                   sf_symbols[SRSRAN_MAX_PORTS]);

SRSRAN_API int srsran_npdsch_encode_rnti(srsran_npdsch_t*        q,
                                         srsran_npdsch_cfg_t*    cfg,
                                         srsran_softbuffer_tx_t* softbuffer,
                                         uint8_t*                data,
                                         uint16_t                rnti,
                                         cf_t*                   sf_symbols[SRSRAN_MAX_PORTS]);

SRSRAN_API int srsran_npdsch_encode_seq(srsran_npdsch_t*        q,
                                        srsran_npdsch_cfg_t*    cfg,
                                        srsran_softbuffer_tx_t* softbuffer,
                                        uint8_t*                data,
                                        srsran_sequence_t*      seq,
                                        cf_t*                   sf_symbols[SRSRAN_MAX_PORTS]);

SRSRAN_API int srsran_npdsch_decode(srsran_npdsch_t*        q,
                                    srsran_npdsch_cfg_t*    cfg,
                                    srsran_softbuffer_rx_t* softbuffer,
                                    cf_t*                   sf_symbols,
                                    cf_t*                   ce[SRSRAN_MAX_PORTS],
                                    float                   noise_estimate,
                                    uint32_t                sfn,
                                    uint8_t*                data);

SRSRAN_API int srsran_npdsch_decode_rnti(srsran_npdsch_t*        q,
                                         srsran_npdsch_cfg_t*    cfg,
                                         srsran_softbuffer_rx_t* softbuffer,
                                         cf_t*                   sf_symbols,
                                         cf_t*                   ce[SRSRAN_MAX_PORTS],
                                         float                   noise_estimate,
                                         uint16_t                rnti,
                                         uint32_t                sfn,
                                         uint8_t*                data,
                                         uint32_t                rep_counter);

SRSRAN_API int
srsran_npdsch_rm_and_decode(srsran_npdsch_t* q, srsran_npdsch_cfg_t* cfg, float* softbits, uint8_t* data);

SRSRAN_API int
srsran_npdsch_cp(srsran_npdsch_t* q, cf_t* input, cf_t* output, srsran_ra_nbiot_dl_grant_t* grant, bool put);

SRSRAN_API float srsran_npdsch_average_noi(srsran_npdsch_t* q);

SRSRAN_API uint32_t srsran_npdsch_last_noi(srsran_npdsch_t* q);

SRSRAN_API void srsran_npdsch_sib1_pack(srsran_cell_t* cell, srsran_sys_info_block_type_1_nb_t* sib, uint8_t* payload);

SRSRAN_API void srsran_npdsch_sib1_unpack(uint8_t* const msg, srsran_sys_info_block_type_1_nb_t* sib);

#endif // SRSRAN_NPDSCH_H
