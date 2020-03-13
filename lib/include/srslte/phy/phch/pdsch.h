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

/******************************************************************************
 *  File:         pdsch.h
 *
 *  Description:  Physical downlink shared channel
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.4
 *****************************************************************************/

#ifndef SRSLTE_PDSCH_H
#define SRSLTE_PDSCH_H

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/modem/evm.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/pdsch_cfg.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/phch/sch.h"
#include "srslte/phy/scrambling/scrambling.h"

typedef struct {
  srslte_sequence_t seq[SRSLTE_MAX_CODEWORDS][SRSLTE_NOF_SF_X_FRAME];
  uint32_t          cell_id;
  bool              sequence_generated;
} srslte_pdsch_user_t;

/* PDSCH object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;

  uint32_t nof_rx_antennas;
  uint32_t max_re;

  uint16_t ue_rnti;
  bool     is_ue;

  bool llr_is_8bit;

  /* buffers */
  // void buffers are shared for tx and rx
  cf_t* ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS]; /* Channel estimation (Rx only) */
  cf_t* symbols[SRSLTE_MAX_PORTS];              /* PDSCH Encoded/Decoded Symbols */
  cf_t* x[SRSLTE_MAX_LAYERS];                   /* Layer mapped */
  cf_t* d[SRSLTE_MAX_CODEWORDS];                /* Modulated/Demodulated codewords */
  void* e[SRSLTE_MAX_CODEWORDS];

  float* csi[SRSLTE_MAX_CODEWORDS]; /* Channel Strengh Indicator */

  /* tx & rx objects */
  srslte_modem_table_t mod[SRSLTE_MOD_NITEMS];

  // EVM buffers, one for each codeword (avoid concurrency issue with coworker)
  srslte_evm_buffer_t* evm_buffer[SRSLTE_MAX_CODEWORDS];

  // This is to generate the scrambling seq for multiple CRNTIs
  srslte_pdsch_user_t** users;

  srslte_sequence_t tmp_seq;

  srslte_sch_t dl_sch;

  void* coworker_ptr;

} srslte_pdsch_t;

typedef struct {
  uint8_t* payload;
  bool     crc;
  float    avg_iterations_block;
  float    evm;
} srslte_pdsch_res_t;

SRSLTE_API int srslte_pdsch_init_ue(srslte_pdsch_t* q, uint32_t max_prb, uint32_t nof_rx_antennas);

SRSLTE_API int srslte_pdsch_init_enb(srslte_pdsch_t* q, uint32_t max_prb);

SRSLTE_API void srslte_pdsch_free(srslte_pdsch_t* q);

/* These functions modify the state of the object and may take some time */
SRSLTE_API int srslte_pdsch_enable_coworker(srslte_pdsch_t* q);

SRSLTE_API int srslte_pdsch_set_cell(srslte_pdsch_t* q, srslte_cell_t cell);

SRSLTE_API int srslte_pdsch_set_rnti(srslte_pdsch_t* q, uint16_t rnti);

SRSLTE_API void srslte_pdsch_free_rnti(srslte_pdsch_t* q, uint16_t rnti);

/* These functions do not modify the state and run in real-time */
SRSLTE_API int srslte_pdsch_encode(srslte_pdsch_t*     q,
                                   srslte_dl_sf_cfg_t* sf,
                                   srslte_pdsch_cfg_t* cfg,
                                   uint8_t*            data[SRSLTE_MAX_CODEWORDS],
                                   cf_t*               sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_pdsch_decode(srslte_pdsch_t*        q,
                                   srslte_dl_sf_cfg_t*    sf,
                                   srslte_pdsch_cfg_t*    cfg,
                                   srslte_chest_dl_res_t* channel,
                                   cf_t*                  sf_symbols[SRSLTE_MAX_PORTS],
                                   srslte_pdsch_res_t     data[SRSLTE_MAX_CODEWORDS]);

SRSLTE_API int srslte_pdsch_select_pmi(srslte_pdsch_t*        q,
                                       srslte_chest_dl_res_t* channel,
                                       uint32_t               nof_layers,
                                       uint32_t*              best_pmi,
                                       float                  sinr[SRSLTE_MAX_CODEBOOKS]);

SRSLTE_API int srslte_pdsch_compute_cn(srslte_pdsch_t* q, srslte_chest_dl_res_t* channel, float* cn);

SRSLTE_API uint32_t srslte_pdsch_grant_rx_info(srslte_pdsch_grant_t* grant,
                                               srslte_pdsch_res_t    res[SRSLTE_MAX_CODEWORDS],
                                               char*                 str,
                                               uint32_t              str_len);

SRSLTE_API uint32_t srslte_pdsch_rx_info(srslte_pdsch_cfg_t* cfg,
                                         srslte_pdsch_res_t  res[SRSLTE_MAX_CODEWORDS],
                                         char*               str,
                                         uint32_t            str_len);

SRSLTE_API uint32_t srslte_pdsch_tx_info(srslte_pdsch_cfg_t* cfg, char* str, uint32_t str_len);

#endif // SRSLTE_PDSCH_H
