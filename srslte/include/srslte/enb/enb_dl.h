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
 *  File:         enb_dl.h
 *
 *  Description:  ENB downlink object.
 *
 *                This module is a frontend to all the downlink data and control
 *                channel processing modules for the ENB transmitter side.
 *
 *  Reference:
 *****************************************************************************/

#ifndef ENBDL_H
#define ENBDL_H

#include <stdbool.h>

#include "srslte/common/phy_common.h"
#include "srslte/dft/ofdm.h"
#include "srslte/sync/pss.h"
#include "srslte/sync/sss.h"
#include "srslte/ch_estimation/refsignal_dl.h"
#include "srslte/phch/dci.h"
#include "srslte/phch/pbch.h"
#include "srslte/phch/pcfich.h"
#include "srslte/phch/pdcch.h"
#include "srslte/phch/pdsch.h"
#include "srslte/phch/pdsch_cfg.h"
#include "srslte/phch/phich.h"
#include "srslte/phch/ra.h"
#include "srslte/phch/regs.h"

#include "srslte/enb/enb_ul.h"

#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"

#include "srslte/config.h"

typedef struct SRSLTE_API {
  srslte_cell_t cell;

  cf_t *sf_symbols[SRSLTE_MAX_PORTS]; 
  cf_t *slot1_symbols[SRSLTE_MAX_PORTS];
  
  srslte_ofdm_t   ifft;
  srslte_pbch_t   pbch;
  srslte_pcfich_t pcfich;
  srslte_regs_t   regs;
  srslte_pdcch_t  pdcch;
  srslte_pdsch_t  pdsch;
  srslte_phich_t  phich; 
  
  srslte_refsignal_cs_t csr_signal;
  srslte_pdsch_cfg_t pdsch_cfg; 
  srslte_ra_dl_dci_t dl_dci;
  
  srslte_dci_format_t dci_format;
  uint32_t cfi;
  
  cf_t pss_signal[SRSLTE_PSS_LEN];
  float sss_signal0[SRSLTE_SSS_LEN]; 
  float sss_signal5[SRSLTE_SSS_LEN]; 
    
  uint32_t nof_rnti;
  
} srslte_enb_dl_t;

typedef struct {
  uint32_t                rnti_idx; 
  srslte_ra_dl_dci_t      grant;
  srslte_dci_location_t   location; 
  srslte_softbuffer_tx_t *softbuffer;
  uint8_t                *data; 
} srslte_enb_dl_pdsch_t; 

typedef struct {
  uint8_t  ack;
  uint32_t rnti_idx; 
} srslte_enb_dl_phich_t; 

/* This function shall be called just after the initial synchronization */
SRSLTE_API int srslte_enb_dl_init(srslte_enb_dl_t *q, 
                                  srslte_cell_t cell, 
                                  uint32_t nof_rntis);

SRSLTE_API void srslte_enb_dl_free(srslte_enb_dl_t *q);

SRSLTE_API void srslte_enb_dl_set_cfi(srslte_enb_dl_t *q, 
                                      uint32_t cfi);

SRSLTE_API void srslte_enb_dl_clear_sf(srslte_enb_dl_t *q);

SRSLTE_API void srslte_enb_dl_put_sync(srslte_enb_dl_t *q, 
                                       uint32_t sf_idx); 

SRSLTE_API void srslte_enb_dl_put_refs(srslte_enb_dl_t *q, 
                                       uint32_t sf_idx);

SRSLTE_API void srslte_enb_dl_put_mib(srslte_enb_dl_t *q, 
                                      uint32_t tti);

SRSLTE_API void srslte_enb_dl_put_pcfich(srslte_enb_dl_t *q, 
                                         uint32_t sf_idx);

SRSLTE_API void srslte_enb_dl_put_phich(srslte_enb_dl_t *q, 
                                        uint8_t ack, 
                                        uint32_t n_prb_lowest, 
                                        uint32_t n_dmrs, 
                                        uint32_t sf_idx);

SRSLTE_API void srslte_enb_dl_put_base(srslte_enb_dl_t *q, 
                                       uint32_t tti);

SRSLTE_API void srslte_enb_dl_gen_signal(srslte_enb_dl_t *q, 
                                         cf_t *signal_buffer); 

SRSLTE_API int srslte_enb_dl_cfg_rnti(srslte_enb_dl_t *q, 
                                      uint32_t idx, 
                                      uint16_t rnti); 

SRSLTE_API int srslte_enb_dl_rem_rnti(srslte_enb_dl_t *q, 
                                      uint32_t idx); 

SRSLTE_API int srslte_enb_dl_put_pdsch(srslte_enb_dl_t *q, 
                                       srslte_ra_dl_grant_t *grant, 
                                       srslte_softbuffer_tx_t *softbuffer,
                                       uint32_t rnti_idx,
                                       uint32_t rv_idx, 
                                       uint32_t sf_idx, 
                                       uint8_t *data); 

SRSLTE_API int srslte_enb_dl_put_pdcch_dl(srslte_enb_dl_t *q, 
                                          srslte_ra_dl_dci_t *grant, 
                                          srslte_dci_format_t format, 
                                          srslte_dci_location_t location,
                                          uint32_t rnti_idx, 
                                          uint32_t sf_idx); 

SRSLTE_API int srslte_enb_dl_put_pdcch_ul(srslte_enb_dl_t *q, 
                                          srslte_ra_ul_dci_t *grant, 
                                          srslte_dci_location_t location,
                                          uint32_t rnti_idx, 
                                          uint32_t sf_idx); 


#endif
