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
 *  File:         pdsch.h
 *
 *  Description:  Physical downlink shared channel
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.4
 *****************************************************************************/

#ifndef SRSRAN_PDSCH_H
#define SRSRAN_PDSCH_H

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/evm.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/pdsch_cfg.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/phch/sch.h"
#include "srsran/phy/scrambling/scrambling.h"

/* PDSCH object */
typedef struct SRSRAN_API {
  srsran_cell_t cell;

  uint32_t nof_rx_antennas;
  uint32_t max_re;

  bool is_ue;

  bool llr_is_8bit;

  /* buffers */
  // void buffers are shared for tx and rx
  cf_t* ce[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS]; /* Channel estimation (Rx only) */
  cf_t* symbols[SRSRAN_MAX_PORTS];              /* PDSCH Encoded/Decoded Symbols */
  cf_t* x[SRSRAN_MAX_LAYERS];                   /* Layer mapped */
  cf_t* d[SRSRAN_MAX_CODEWORDS];                /* Modulated/Demodulated codewords */
  void* e[SRSRAN_MAX_CODEWORDS];

  float* csi[SRSRAN_MAX_CODEWORDS]; /* Channel Strengh Indicator */

  /* tx & rx objects */
  srsran_modem_table_t mod[SRSRAN_MOD_NITEMS];

  // EVM buffers, one for each codeword (avoid concurrency issue with coworker)
  srsran_evm_buffer_t* evm_buffer[SRSRAN_MAX_CODEWORDS];
  float                avg_evm;

  srsran_sch_t dl_sch;

  void* coworker_ptr;

} srsran_pdsch_t;

typedef struct {
  uint8_t* payload;
  bool     crc;
  float    avg_iterations_block;
  float    evm;
} srsran_pdsch_res_t;

SRSRAN_API int srsran_pdsch_init_ue(srsran_pdsch_t* q, uint32_t max_prb, uint32_t nof_rx_antennas);

SRSRAN_API int srsran_pdsch_init_enb(srsran_pdsch_t* q, uint32_t max_prb);

SRSRAN_API void srsran_pdsch_free(srsran_pdsch_t* q);

/* These functions modify the state of the object and may take some time */
SRSRAN_API int srsran_pdsch_enable_coworker(srsran_pdsch_t* q);

SRSRAN_API int srsran_pdsch_set_cell(srsran_pdsch_t* q, srsran_cell_t cell);

/* These functions do not modify the state and run in real-time */
SRSRAN_API int srsran_pdsch_encode(srsran_pdsch_t*     q,
                                   srsran_dl_sf_cfg_t* sf,
                                   srsran_pdsch_cfg_t* cfg,
                                   uint8_t*            data[SRSRAN_MAX_CODEWORDS],
                                   cf_t*               sf_symbols[SRSRAN_MAX_PORTS]);

SRSRAN_API int srsran_pdsch_decode(srsran_pdsch_t*        q,
                                   srsran_dl_sf_cfg_t*    sf,
                                   srsran_pdsch_cfg_t*    cfg,
                                   srsran_chest_dl_res_t* channel,
                                   cf_t*                  sf_symbols[SRSRAN_MAX_PORTS],
                                   srsran_pdsch_res_t     data[SRSRAN_MAX_CODEWORDS]);

SRSRAN_API int srsran_pdsch_select_pmi(srsran_pdsch_t*        q,
                                       srsran_chest_dl_res_t* channel,
                                       uint32_t               nof_layers,
                                       uint32_t*              best_pmi,
                                       float                  sinr[SRSRAN_MAX_CODEBOOKS]);

SRSRAN_API int srsran_pdsch_compute_cn(srsran_pdsch_t* q, srsran_chest_dl_res_t* channel, float* cn);

SRSRAN_API uint32_t srsran_pdsch_grant_rx_info(srsran_pdsch_grant_t* grant,
                                               srsran_pdsch_res_t    res[SRSRAN_MAX_CODEWORDS],
                                               char*                 str,
                                               uint32_t              str_len);

SRSRAN_API uint32_t srsran_pdsch_rx_info(srsran_pdsch_cfg_t* cfg,
                                         srsran_pdsch_res_t  res[SRSRAN_MAX_CODEWORDS],
                                         char*               str,
                                         uint32_t            str_len);

SRSRAN_API uint32_t srsran_pdsch_tx_info(srsran_pdsch_cfg_t* cfg, char* str, uint32_t str_len);

#endif // SRSRAN_PDSCH_H
