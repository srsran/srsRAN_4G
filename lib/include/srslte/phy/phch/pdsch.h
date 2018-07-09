/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/scrambling/scrambling.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/phch/sch.h"
#include "srslte/phy/phch/pdsch_cfg.h"

typedef struct {
  srslte_sequence_t seq[SRSLTE_MAX_CODEWORDS][SRSLTE_NSUBFRAMES_X_FRAME];
  uint32_t cell_id;
  bool sequence_generated;
} srslte_pdsch_user_t;

/* PDSCH object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;
  
  uint32_t nof_rx_antennas;
  uint32_t last_nof_iterations[SRSLTE_MAX_CODEWORDS];

  uint32_t max_re;

  uint16_t ue_rnti;
  bool is_ue;

  /* Power allocation parameter 3GPP 36.213 Clause 5.2 Rho_b */
  float rho_a;

  /* buffers */
  // void buffers are shared for tx and rx
  cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS]; /* Channel estimation (Rx only) */
  cf_t *symbols[SRSLTE_MAX_PORTS];              /* PDSCH Encoded/Decoded Symbols */
  cf_t *x[SRSLTE_MAX_LAYERS];                   /* Layer mapped */
  cf_t *d[SRSLTE_MAX_CODEWORDS];                /* Modulated/Demodulated codewords */
  void *e[SRSLTE_MAX_CODEWORDS];

  bool csi_enabled;
  float *csi[SRSLTE_MAX_CODEWORDS];             /* Channel Strengh Indicator */

  /* tx & rx objects */
  srslte_modem_table_t mod[4];
  
  // This is to generate the scrambling seq for multiple CRNTIs
  srslte_pdsch_user_t **users;

  srslte_sequence_t tmp_seq;

  srslte_sch_t dl_sch;

  void *coworker_ptr;

} srslte_pdsch_t;


SRSLTE_API int srslte_pdsch_init_ue(srslte_pdsch_t *q,
                                    uint32_t max_prb,
                                    uint32_t nof_rx_antennas);

SRSLTE_API int srslte_pdsch_init_enb(srslte_pdsch_t *q,
                                     uint32_t max_prb);

SRSLTE_API void srslte_pdsch_free(srslte_pdsch_t *q);

SRSLTE_API int srslte_pdsch_set_cell(srslte_pdsch_t *q,
                                     srslte_cell_t cell);

SRSLTE_API int srslte_pdsch_set_rnti(srslte_pdsch_t *q,
                                     uint16_t rnti);

SRSLTE_API void srslte_pdsch_set_power_allocation(srslte_pdsch_t *q,
                                                  float rho_a);

SRSLTE_API int srslte_pdsch_enable_csi(srslte_pdsch_t *q,
                                       bool enable);

SRSLTE_API void srslte_pdsch_free_rnti(srslte_pdsch_t *q, 
                                      uint16_t rnti);

SRSLTE_API int srslte_pdsch_cfg(srslte_pdsch_cfg_t *cfg,
                                srslte_cell_t cell, 
                                srslte_ra_dl_grant_t *grant, 
                                uint32_t cfi, 
                                uint32_t sf_idx, 
                                int rvidx);

SRSLTE_API int srslte_pdsch_cfg_mimo(srslte_pdsch_cfg_t *cfg,
                                     srslte_cell_t cell,
                                     srslte_ra_dl_grant_t *grant,
                                     uint32_t cfi,
                                     uint32_t sf_idx,
                                     int rvidx[SRSLTE_MAX_CODEWORDS],
                                     srslte_mimo_type_t mimo_type,
                                     uint32_t pmi);

SRSLTE_API int srslte_pdsch_encode(srslte_pdsch_t *q,
                                         srslte_pdsch_cfg_t *cfg,
                                         srslte_softbuffer_tx_t *softbuffers[SRSLTE_MAX_CODEWORDS],
                                         uint8_t *data[SRSLTE_MAX_CODEWORDS],
                                         uint16_t rnti,
                                         cf_t *sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_pdsch_decode(srslte_pdsch_t *q, 
                                   srslte_pdsch_cfg_t *cfg,
                                   srslte_softbuffer_rx_t *softbuffers[SRSLTE_MAX_CODEWORDS],
                                   cf_t *sf_symbols[SRSLTE_MAX_PORTS],
                                   cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                   float noise_estimate,
                                   uint16_t rnti,
                                   uint8_t *data[SRSLTE_MAX_CODEWORDS],
                                   bool acks[SRSLTE_MAX_CODEWORDS]);

SRSLTE_API int srslte_pdsch_pmi_select(srslte_pdsch_t *q,
                                       srslte_pdsch_cfg_t *cfg,
                                       cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                       float noise_estimate,
                                       uint32_t nof_ce,
                                       uint32_t pmi[SRSLTE_MAX_LAYERS],
                                       float sinr[SRSLTE_MAX_LAYERS][SRSLTE_MAX_CODEBOOKS]);

SRSLTE_API int srslte_pdsch_cn_compute(srslte_pdsch_t *q,
                                       cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                       uint32_t nof_ce,
                                       float *cn);

SRSLTE_API void srslte_pdsch_set_max_noi(srslte_pdsch_t *q,
                                         uint32_t max_iter);

SRSLTE_API float srslte_pdsch_last_noi(srslte_pdsch_t *q);

SRSLTE_API int srslte_pdsch_enable_coworker(srslte_pdsch_t *q);

SRSLTE_API uint32_t srslte_pdsch_last_noi_cw(srslte_pdsch_t *q,
                                             uint32_t cw_idx);

#endif // SRSLTE_PDSCH_H
