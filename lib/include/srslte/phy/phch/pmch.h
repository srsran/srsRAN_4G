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
 *  File:         pmch.h
 *
 *  Description:  Physical multicast channel
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.5
 *****************************************************************************/

#ifndef SRSLTE_PMCH_H
#define SRSLTE_PMCH_H

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
#include "srslte/phy/common/sequence.h"

typedef struct {
  srslte_sequence_t seq[SRSLTE_NSUBFRAMES_X_FRAME];  
} srslte_pmch_seq_t;

typedef struct SRSLTE_API {
  srslte_cbsegm_t cb_segm;
  srslte_ra_dl_grant_t grant;
  srslte_ra_nbits_t nbits[SRSLTE_MAX_CODEWORDS];
  uint32_t sf_idx;
} srslte_pmch_cfg_t;

/* PMCH object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;
  
  uint32_t nof_rx_antennas;
  
  uint32_t max_re;
  
  /* buffers */
  // void buffers are shared for tx and rx
  cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  cf_t *symbols[SRSLTE_MAX_PORTS];
  cf_t *x[SRSLTE_MAX_PORTS];
  cf_t *d;
  void *e;

  /* tx & rx objects */
  srslte_modem_table_t mod[4];
  
  // This is to generate the scrambling seq for multiple MBSFN Area IDs
  srslte_pmch_seq_t **seqs;
  
  srslte_sch_t dl_sch;
  
} srslte_pmch_t;


SRSLTE_API int srslte_pmch_init(srslte_pmch_t *q,
                                 uint32_t max_prb);

SRSLTE_API int srslte_pmch_init_multi(srslte_pmch_t *q,
                                       uint32_t max_prb, 
                                       uint32_t nof_rx_antennas);

SRSLTE_API void srslte_pmch_free(srslte_pmch_t *q);

SRSLTE_API int srslte_pmch_set_cell(srslte_pmch_t *q, srslte_cell_t cell);

SRSLTE_API int srslte_pmch_set_area_id(srslte_pmch_t *q, uint16_t area_id);

SRSLTE_API void srslte_pmch_free_area_id(srslte_pmch_t *q, uint16_t area_id);



SRSLTE_API int srslte_pmch_get(srslte_pmch_t *q, cf_t *sf_symbols, cf_t *symbols, uint32_t lstart);

SRSLTE_API int srslte_pmch_put(srslte_pmch_t *q, cf_t *symbols, cf_t *sf_symbols, uint32_t lstart);

SRSLTE_API int srslte_pmch_cp(srslte_pmch_t *q, cf_t *input, cf_t *output, uint32_t lstart_grant, bool put);



SRSLTE_API float srslte_pmch_coderate(uint32_t tbs,
                                       uint32_t nof_re); 


SRSLTE_API int srslte_pmch_cfg(srslte_pdsch_cfg_t *cfg,
                               srslte_cell_t cell,
                               srslte_ra_dl_grant_t *grant,
                               uint32_t cfi,
                               uint32_t sf_idx);

SRSLTE_API int srslte_pmch_encode(srslte_pmch_t *q,
                                   srslte_pdsch_cfg_t *cfg,
                                   srslte_softbuffer_tx_t *softbuffer,
                                   uint8_t *data, 
                                   uint16_t area_id,
                                   cf_t *sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_pmch_decode(srslte_pmch_t *q,
                                   srslte_pdsch_cfg_t *cfg,
                                   srslte_softbuffer_rx_t *softbuffer,
                                   cf_t *sf_symbols, 
                                   cf_t *ce[SRSLTE_MAX_PORTS],
                                   float noise_estimate, 
                                   uint16_t area_id,
                                   uint8_t *data);

SRSLTE_API int srslte_pmch_decode_multi(srslte_pmch_t *q,
                                         srslte_pdsch_cfg_t *cfg,
                                         srslte_softbuffer_rx_t *softbuffer,
                                         cf_t *sf_symbols[SRSLTE_MAX_PORTS], 
                                         cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                         float noise_estimate, 
                                         uint16_t area_id,
                                         uint8_t *data);

SRSLTE_API float srslte_pmch_average_noi(srslte_pmch_t *q);

SRSLTE_API uint32_t srslte_pmch_last_noi(srslte_pmch_t *q);

#endif // SRSLTE_PMCH_H
