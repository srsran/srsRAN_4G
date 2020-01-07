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

#ifndef SRSLTE_NPDSCH_H
#define SRSLTE_NPDSCH_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/fec/convcoder.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/npdsch_cfg.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/phch/sch.h"
#include "srslte/phy/scrambling/scrambling.h"

#define SRSLTE_NPDSCH_MAX_RE                                                                                           \
  (SRSLTE_CP_NORM_SF_NSYMB * SRSLTE_NRE - 8) ///< Full PRB minus 8 RE for NRS (one antenna port)
#define SRSLTE_NPDSCH_MAX_TBS 680            ///< Max TBS in Rel13 NB-IoT
#define SRSLTE_NPDSCH_CRC_LEN (24)
#define SRSLTE_NPDSCH_MAX_TBS_CRC (SRSLTE_NPDSCH_MAX_TBS + SRSLTE_NPDSCH_CRC_LEN)
#define SRSLTE_NPDSCH_MAX_TBS_ENC (3 * SRSLTE_NPDSCH_MAX_TBS_CRC)
#define SRSLTE_NPDSCH_MAX_NOF_SF 10
#define SRSLTE_NPDSCH_NUM_SEQ (2 * SRSLTE_NOF_SF_X_FRAME) ///< for even and odd numbered SFNs

/* @brief Narrowband Physical Downlink shared channel (NPDSCH)
 *
 * Reference:    3GPP TS 36.211 version 13.2.0 Release 13 Sec. 10.2.3
 */
typedef struct SRSLTE_API {
  srslte_nbiot_cell_t cell;
  uint32_t            max_re;
  bool                rnti_is_set;
  uint16_t            rnti;

  // buffers
  uint8_t data[SRSLTE_NPDSCH_MAX_TBS_CRC];
  uint8_t data_enc[SRSLTE_NPDSCH_MAX_TBS_ENC];
  float   rm_f[SRSLTE_NPDSCH_MAX_TBS_ENC];
  cf_t*   ce[SRSLTE_MAX_PORTS];
  cf_t*   symbols[SRSLTE_MAX_PORTS];
  cf_t*   sib_symbols[SRSLTE_MAX_PORTS]; // extra buffer for SIB1 symbols as they may be interleaved with other NPDSCH
  cf_t*   tx_syms[SRSLTE_MAX_PORTS];     // pointer to either symbols or sib1_symbols
  cf_t*   x[SRSLTE_MAX_PORTS];
  cf_t*   d;

  float*   llr;
  uint8_t* temp;
  uint8_t* rm_b;

  // tx & rx objects
  srslte_modem_table_t mod;
  srslte_viterbi_t     decoder;
  srslte_sequence_t    seq[SRSLTE_NPDSCH_NUM_SEQ];
  srslte_crc_t         crc;
  srslte_convcoder_t   encoder;
} srslte_npdsch_t;

typedef struct {
  uint16_t hyper_sfn;
  // TODO: add all other fields
} srslte_sys_info_block_type_1_nb_t;

SRSLTE_API int srslte_npdsch_init(srslte_npdsch_t* q);

SRSLTE_API void srslte_npdsch_free(srslte_npdsch_t* q);

SRSLTE_API int srslte_npdsch_set_cell(srslte_npdsch_t* q, srslte_nbiot_cell_t cell);

SRSLTE_API int srslte_npdsch_set_rnti(srslte_npdsch_t* q, uint16_t rnti);

SRSLTE_API int srslte_npdsch_cfg(srslte_npdsch_cfg_t*        cfg,
                                 srslte_nbiot_cell_t         cell,
                                 srslte_ra_nbiot_dl_grant_t* grant,
                                 uint32_t                    sf_idx);

SRSLTE_API int srslte_npdsch_encode(srslte_npdsch_t*        q,
                                    srslte_npdsch_cfg_t*    cfg,
                                    srslte_softbuffer_tx_t* softbuffer,
                                    uint8_t*                data,
                                    cf_t*                   sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_npdsch_encode_rnti_idx(srslte_npdsch_t*        q,
                                             srslte_npdsch_cfg_t*    cfg,
                                             srslte_softbuffer_tx_t* softbuffer,
                                             uint8_t*                data,
                                             uint32_t                rnti_idx,
                                             cf_t*                   sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_npdsch_encode_rnti(srslte_npdsch_t*        q,
                                         srslte_npdsch_cfg_t*    cfg,
                                         srslte_softbuffer_tx_t* softbuffer,
                                         uint8_t*                data,
                                         uint16_t                rnti,
                                         cf_t*                   sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_npdsch_encode_seq(srslte_npdsch_t*        q,
                                        srslte_npdsch_cfg_t*    cfg,
                                        srslte_softbuffer_tx_t* softbuffer,
                                        uint8_t*                data,
                                        srslte_sequence_t*      seq,
                                        cf_t*                   sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_npdsch_decode(srslte_npdsch_t*        q,
                                    srslte_npdsch_cfg_t*    cfg,
                                    srslte_softbuffer_rx_t* softbuffer,
                                    cf_t*                   sf_symbols,
                                    cf_t*                   ce[SRSLTE_MAX_PORTS],
                                    float                   noise_estimate,
                                    uint32_t                sfn,
                                    uint8_t*                data);

SRSLTE_API int srslte_npdsch_decode_rnti(srslte_npdsch_t*        q,
                                         srslte_npdsch_cfg_t*    cfg,
                                         srslte_softbuffer_rx_t* softbuffer,
                                         cf_t*                   sf_symbols,
                                         cf_t*                   ce[SRSLTE_MAX_PORTS],
                                         float                   noise_estimate,
                                         uint16_t                rnti,
                                         uint32_t                sfn,
                                         uint8_t*                data,
                                         uint32_t                rep_counter);

SRSLTE_API int
srslte_npdsch_rm_and_decode(srslte_npdsch_t* q, srslte_npdsch_cfg_t* cfg, float* softbits, uint8_t* data);

SRSLTE_API int
srslte_npdsch_cp(srslte_npdsch_t* q, cf_t* input, cf_t* output, srslte_ra_nbiot_dl_grant_t* grant, bool put);

SRSLTE_API float srslte_npdsch_average_noi(srslte_npdsch_t* q);

SRSLTE_API uint32_t srslte_npdsch_last_noi(srslte_npdsch_t* q);

SRSLTE_API void srslte_npdsch_sib1_pack(srslte_cell_t* cell, srslte_sys_info_block_type_1_nb_t* sib, uint8_t* payload);

SRSLTE_API void srslte_npdsch_sib1_unpack(uint8_t* const msg, srslte_sys_info_block_type_1_nb_t* sib);

#endif // SRSLTE_NPDSCH_H
