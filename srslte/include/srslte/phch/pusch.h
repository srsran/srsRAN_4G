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
 *  File:         pusch.h
 *
 *  Description:  Physical uplink shared channel.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.3
 *****************************************************************************/

#ifndef PUSCH_
#define PUSCH_

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/mimo/precoding.h"
#include "srslte/mimo/layermap.h"
#include "srslte/modem/mod.h"
#include "srslte/modem/demod_soft.h"
#include "srslte/scrambling/scrambling.h"
#include "srslte/phch/regs.h"
#include "srslte/phch/dci.h"
#include "srslte/phch/sch.h"
#include "srslte/phch/pusch_cfg.h"
#include "srslte/dft/dft_precoding.h"
#include "srslte/ch_estimation/refsignal_ul.h"

#define SRSLTE_PUSCH_MAX_TDEC_ITERS         5




typedef struct {
  enum {
    SRSLTE_PUSCH_HOP_MODE_INTER_SF = 1,
    SRSLTE_PUSCH_HOP_MODE_INTRA_SF = 0
  } hop_mode; 
  uint32_t hopping_offset;
  uint32_t n_sb;
} srslte_pusch_hopping_cfg_t;

/* PUSCH object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;

  uint32_t max_re;
  bool rnti_is_set; 
  uint16_t rnti; 

  srslte_dft_precoding_t dft_precoding;  
  
  /* buffers */
  // void buffers are shared for tx and rx
  cf_t *ce;
  cf_t *z;
  cf_t *d;

  void *q;
  void *g;

  /* tx & rx objects */
  srslte_modem_table_t mod[4];
  srslte_sequence_t seq[SRSLTE_NSUBFRAMES_X_FRAME];
  srslte_sequence_t seq_type2_fo; 
  
  // This is to generate the scrambling seq for multiple CRNTIs
  uint32_t nof_crnti; 
  srslte_sequence_t *seq_multi[SRSLTE_NSUBFRAMES_X_FRAME];
  uint16_t *rnti_multi;

  srslte_sch_t ul_sch;
  bool shortened;
  
}srslte_pusch_t;


SRSLTE_API int srslte_pusch_init(srslte_pusch_t *q, 
                                 srslte_cell_t cell);

SRSLTE_API void srslte_pusch_free(srslte_pusch_t *q);

SRSLTE_API int srslte_pusch_cfg(srslte_pusch_t             *q, 
                                srslte_pusch_cfg_t         *cfg, 
                                srslte_ra_ul_grant_t       *grant, 
                                srslte_uci_cfg_t           *uci_cfg, 
                                srslte_pusch_hopping_cfg_t *hopping_cfg, 
                                srslte_refsignal_srs_cfg_t *srs_cfg, 
                                uint32_t tti, 
                                uint32_t rv_idx, 
                                uint32_t current_tx_nb); 

SRSLTE_API int srslte_pusch_set_rnti(srslte_pusch_t *q, 
                                     uint16_t rnti);

SRSLTE_API int srslte_pusch_init_rnti_multi(srslte_pusch_t *q, 
                                            uint32_t nof_rntis); 

SRSLTE_API int srslte_pusch_set_rnti_multi(srslte_pusch_t *q, 
                                           uint32_t idx,
                                           uint16_t rnti);

SRSLTE_API uint16_t srslte_pusch_get_rnti_multi(srslte_pusch_t *q, 
                                                uint32_t idx);


SRSLTE_API int srslte_pusch_encode(srslte_pusch_t *q, 
                                   srslte_pusch_cfg_t *cfg,
                                   srslte_softbuffer_tx_t *softbuffer,
                                   uint8_t *data, 
                                   cf_t *sf_symbols);

SRSLTE_API int srslte_pusch_encode_rnti(srslte_pusch_t *q, 
                                        srslte_pusch_cfg_t *cfg,
                                        srslte_softbuffer_tx_t *softbuffer,
                                        uint8_t *data, 
                                        uint16_t rnti, 
                                        cf_t *sf_symbols); 

SRSLTE_API int srslte_pusch_uci_encode(srslte_pusch_t *q, 
                                       srslte_pusch_cfg_t *cfg,
                                       srslte_softbuffer_tx_t *softbuffer,
                                       uint8_t *data, 
                                       srslte_uci_data_t uci_data, 
                                       cf_t *sf_symbols);

SRSLTE_API int srslte_pusch_uci_encode_rnti(srslte_pusch_t *q, 
                                            srslte_pusch_cfg_t *cfg,
                                            srslte_softbuffer_tx_t *softbuffer,
                                            uint8_t *data, 
                                            srslte_uci_data_t uci_data, 
                                            uint16_t rnti, 
                                            cf_t *sf_symbols); 

SRSLTE_API int srslte_pusch_decode(srslte_pusch_t *q, 
                                   srslte_pusch_cfg_t *cfg,
                                   srslte_softbuffer_rx_t *softbuffer,
                                   cf_t *sf_symbols, 
                                   cf_t *ce,
                                   float noise_estimate, 
                                   uint8_t *data);

SRSLTE_API int srslte_pusch_uci_decode(srslte_pusch_t *q, 
                                       srslte_pusch_cfg_t *cfg,
                                       srslte_softbuffer_rx_t *softbuffer,
                                       cf_t *sf_symbols, 
                                       cf_t *ce,
                                       float noise_estimate, 
                                       uint8_t *data, 
                                       srslte_uci_data_t *uci_data);

SRSLTE_API int srslte_pusch_uci_decode_rnti_idx(srslte_pusch_t *q, 
                                                srslte_pusch_cfg_t *cfg,
                                                srslte_softbuffer_rx_t *softbuffer,
                                                cf_t *sf_symbols, 
                                                cf_t *ce,
                                                float noise_estimate, 
                                                uint32_t rnti_idx,
                                                uint8_t *data, 
                                                srslte_uci_data_t *uci_data);

SRSLTE_API float srslte_pusch_average_noi(srslte_pusch_t *q); 

SRSLTE_API uint32_t srslte_pusch_last_noi(srslte_pusch_t *q); 

#endif
