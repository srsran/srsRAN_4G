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

#ifndef SRSLTE_ENB_DL_H
#define SRSLTE_ENB_DL_H

#include <stdbool.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/sync/pss.h"
#include "srslte/phy/sync/sss.h"
#include "srslte/phy/ch_estimation/refsignal_dl.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/pbch.h"
#include "srslte/phy/phch/pcfich.h"
#include "srslte/phy/phch/pdcch.h"
#include "srslte/phy/phch/pdsch.h"
#include "srslte/phy/phch/pdsch_cfg.h"
#include "srslte/phy/phch/phich.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/phch/regs.h"

#include "srslte/phy/enb/enb_ul.h"

#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/debug.h"

#include "srslte/config.h"

typedef struct SRSLTE_API {
  srslte_cell_t cell;

  cf_t *sf_symbols[SRSLTE_MAX_PORTS]; 
  cf_t *slot1_symbols[SRSLTE_MAX_PORTS];
  
  srslte_ofdm_t   ifft[SRSLTE_MAX_PORTS];
  srslte_pbch_t   pbch;
  srslte_pcfich_t pcfich;
  srslte_regs_t   regs;
  srslte_pdcch_t  pdcch;
  srslte_pdsch_t  pdsch;
  srslte_phich_t  phich; 
  
  srslte_refsignal_t csr_signal;
  srslte_pdsch_cfg_t pdsch_cfg; 
  srslte_ra_dl_dci_t dl_dci;
  
  srslte_dci_format_t dci_format;
  uint32_t cfi;
  
  cf_t pss_signal[SRSLTE_PSS_LEN];
  float sss_signal0[SRSLTE_SSS_LEN]; 
  float sss_signal5[SRSLTE_SSS_LEN]; 
    
  float tx_amp;
  float rho_b;

  uint8_t tmp[1024*128];
  
} srslte_enb_dl_t;

typedef struct {
  uint16_t                rnti; 
  srslte_dci_format_t     dci_format;
  srslte_ra_dl_dci_t      grant;
  srslte_dci_location_t   location; 
  srslte_softbuffer_tx_t *softbuffers[SRSLTE_MAX_TB];
  uint8_t                *data[SRSLTE_MAX_TB];
} srslte_enb_dl_pdsch_t; 

typedef struct {
  uint16_t rnti; 
  uint8_t  ack;
  uint32_t n_prb_lowest;
  uint32_t n_dmrs;  
} srslte_enb_dl_phich_t; 

/* This function shall be called just after the initial synchronization */
SRSLTE_API int srslte_enb_dl_init(srslte_enb_dl_t *q,
                                  cf_t *out_buffer[SRSLTE_MAX_PORTS],
                                  uint32_t max_prb);

SRSLTE_API void srslte_enb_dl_free(srslte_enb_dl_t *q);

SRSLTE_API int srslte_enb_dl_set_cell(srslte_enb_dl_t *q,
                                      srslte_cell_t cell);

SRSLTE_API void srslte_enb_dl_set_cfi(srslte_enb_dl_t *q,
                                      uint32_t cfi);

SRSLTE_API void srslte_enb_dl_set_power_allocation(srslte_enb_dl_t *q,
                                                   float rho_a,
                                                   float rho_b);

SRSLTE_API void srslte_enb_dl_apply_power_allocation(srslte_enb_dl_t *q);

SRSLTE_API void srslte_enb_dl_prepare_power_allocation(srslte_enb_dl_t *q);

SRSLTE_API void srslte_enb_dl_set_amp(srslte_enb_dl_t *q, 
                                      float amp); 

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

SRSLTE_API void srslte_enb_dl_gen_signal(srslte_enb_dl_t *q);

SRSLTE_API int srslte_enb_dl_add_rnti(srslte_enb_dl_t *q, 
                                      uint16_t rnti); 

SRSLTE_API void srslte_enb_dl_rem_rnti(srslte_enb_dl_t *q, 
                                      uint16_t rnti); 

SRSLTE_API int srslte_enb_dl_put_pdsch(srslte_enb_dl_t *q, 
                                       srslte_ra_dl_grant_t *grant, 
                                       srslte_softbuffer_tx_t *softbuffer[SRSLTE_MAX_CODEWORDS],
                                       uint16_t rnti,
                                       int rv_idx[SRSLTE_MAX_CODEWORDS],
                                       uint32_t sf_idx, 
                                       uint8_t *data[SRSLTE_MAX_CODEWORDS],
                                       srslte_mimo_type_t mimo_type);

SRSLTE_API int srslte_enb_dl_put_pdcch_dl(srslte_enb_dl_t *q, 
                                          srslte_ra_dl_dci_t *grant, 
                                          srslte_dci_format_t format, 
                                          srslte_dci_location_t location,
                                          uint16_t rnti, 
                                          uint32_t sf_idx);

SRSLTE_API int srslte_enb_dl_put_pdcch_ul(srslte_enb_dl_t *q, 
                                          srslte_ra_ul_dci_t *grant, 
                                          srslte_dci_location_t location,
                                          uint16_t rnti, 
                                          uint32_t sf_idx); 

SRSLTE_API void srslte_enb_dl_save_signal(srslte_enb_dl_t *q,
                                          srslte_softbuffer_tx_t *softbuffer,
                                          uint8_t *data,
                                          uint32_t tti,
                                          uint32_t rv_idx,
                                          uint16_t rnti,
                                          uint32_t cfi);

#endif // SRSLTE_ENB_DL_H
