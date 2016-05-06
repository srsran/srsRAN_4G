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

#ifndef PDSCH_
#define PDSCH_

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/mimo/precoding.h"
#include "srslte/mimo/layermap.h"
#include "srslte/modem/mod.h"
#include "srslte/modem/demod_soft.h"
#include "srslte/scrambling/scrambling.h"
#include "srslte/phch/dci.h"
#include "srslte/phch/regs.h"
#include "srslte/phch/sch.h"
#include "srslte/phch/pdsch_cfg.h"

/* PDSCH object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;
  
  uint32_t max_re;
  bool rnti_is_set; 
  uint16_t rnti; 
  
  /* buffers */
  // void buffers are shared for tx and rx
  cf_t *ce[SRSLTE_MAX_PORTS];
  cf_t *symbols[SRSLTE_MAX_PORTS];
  cf_t *x[SRSLTE_MAX_PORTS];
  cf_t *d;
  void *e;

  /* tx & rx objects */
  srslte_modem_table_t mod[4];
  
  srslte_sequence_t seq[SRSLTE_NSUBFRAMES_X_FRAME];
  
  // This is to generate the scrambling seq for multiple CRNTIs
  uint32_t nof_crnti; 
  srslte_sequence_t *seq_multi[SRSLTE_NSUBFRAMES_X_FRAME];
  uint16_t *rnti_multi;
  
  srslte_sch_t dl_sch;
  
} srslte_pdsch_t;

SRSLTE_API int srslte_pdsch_init(srslte_pdsch_t *q, 
                                 srslte_cell_t cell);

SRSLTE_API void srslte_pdsch_free(srslte_pdsch_t *q);

SRSLTE_API int srslte_pdsch_set_rnti(srslte_pdsch_t *q, 
                                     uint16_t rnti);

SRSLTE_API int srslte_pdsch_init_rnti_multi(srslte_pdsch_t *q, 
                                            uint32_t nof_rntis); 

SRSLTE_API int srslte_pdsch_set_rnti_multi(srslte_pdsch_t *q, 
                                           uint32_t idx,
                                           uint16_t rnti);

SRSLTE_API uint16_t srslte_pdsch_get_rnti_multi(srslte_pdsch_t *q, 
                                                uint32_t idx);

SRSLTE_API int srslte_pdsch_cfg(srslte_pdsch_cfg_t *cfg, 
                                srslte_cell_t cell, 
                                srslte_ra_dl_grant_t *grant, 
                                uint32_t cfi, 
                                uint32_t sf_idx, 
                                uint32_t rvidx); 

SRSLTE_API int srslte_pdsch_encode(srslte_pdsch_t *q,
                                   srslte_pdsch_cfg_t *cfg,
                                   srslte_softbuffer_tx_t *softbuffer,
                                   uint8_t *data, 
                                   cf_t *sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_pdsch_encode_rnti_idx(srslte_pdsch_t *q,
                                            srslte_pdsch_cfg_t *cfg,
                                            srslte_softbuffer_tx_t *softbuffer,
                                            uint8_t *data, 
                                            uint32_t rnti_idx,
                                            cf_t *sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_pdsch_encode_rnti(srslte_pdsch_t *q,
                                        srslte_pdsch_cfg_t *cfg,
                                        srslte_softbuffer_tx_t *softbuffer,
                                        uint8_t *data, 
                                        uint16_t rnti,
                                        cf_t *sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_pdsch_decode(srslte_pdsch_t *q, 
                                   srslte_pdsch_cfg_t *cfg, 
                                   srslte_softbuffer_rx_t *softbuffer,
                                   cf_t *sf_symbols, 
                                   cf_t *ce[SRSLTE_MAX_PORTS],
                                   float noise_estimate, 
                                   uint8_t *data);

SRSLTE_API int srslte_pdsch_decode_rnti(srslte_pdsch_t *q, 
                                        srslte_pdsch_cfg_t *cfg, 
                                        srslte_softbuffer_rx_t *softbuffer,
                                        cf_t *sf_symbols, 
                                        cf_t *ce[SRSLTE_MAX_PORTS],
                                        float noise_estimate, 
                                        uint16_t rnti,
                                        uint8_t *data);

SRSLTE_API float srslte_pdsch_average_noi(srslte_pdsch_t *q); 

SRSLTE_API uint32_t srslte_pdsch_last_noi(srslte_pdsch_t *q); 

#endif
